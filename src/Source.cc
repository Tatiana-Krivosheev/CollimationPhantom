#include <tuple>
#include <limits>
#include <fstream>

#include "Source.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "SourceMessenger.hh"
#include "globals.hh"

#include "Randomize.hh"

using nl = std::numeric_limits<float>;

static inline float degree_to_radian(float adegree)
{
    return adegree * float(M_PI) / 180.0f;
}

// here you set global source parameters, called once per run
Source::Source():
    _particleGun{nullptr},
    _sourceMessenger{nullptr},

    _iso_radius{nl::quiet_NaN()},
    _src_angle{nl::quiet_NaN()},
    _polar_start{nl::quiet_NaN()},
    _polar_stop{nl::quiet_NaN()},
    _shift_x{nl::quiet_NaN()},
    _shift_y{nl::quiet_NaN()},
    _shift_z{nl::quiet_NaN()},

    _gamma{nullptr},
    _electron{nullptr},
    _positron{nullptr},
    _geantino{nullptr}
{
    _particleGun = new G4ParticleGun{ 1 };

    auto* particleTable = G4ParticleTable::GetParticleTable();
    _gamma    = particleTable->FindParticle("gamma");
    _electron = particleTable->FindParticle("e-");
    _positron = particleTable->FindParticle("e+");
    _geantino = particleTable->FindParticle("geantino");

    _particleGun->SetParticleDefinition(_gamma);

    _particleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
    _particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    _particleGun->SetParticleEnergy(1000.0*MeV);

    _sourceMessenger = new SourceMessenger(this);
}

Source::~Source()
{
    delete _particleGun;
    delete _sourceMessenger;
}

void Source::set_src_angle(float angle)
{
    G4cout << "Source::set_src_angle: " << angle << G4endl;
    _src_angle = angle;

    _polar_start = cos(angle);
    _polar_stop  = cos(0.0);
}

void Source::set_sources(const std::string& fname)
{
    G4cout << "Source::set_sources " << fname << G4endl;

    std::ifstream is(fname);
    if (not is.is_open()) {
        throw 1;
    }

    float lat, lon; // source latitude and longitude
    std::vector<angles> srcs;
    srcs.reserve(200);
    while (not is.eof()) {
        is >> lat >> lon;
        srcs.emplace_back(angles(lat, lon));
    }

    this->set_sources(srcs);
}

void Source::set_sources(const std::vector<angles>& srcs)
{
    _srcs.clear();
    _srcs.reserve(srcs.size());
    for(const auto& ss: srcs)
    {
        auto theta = degree_to_radian( ss.first );
        auto phi   = degree_to_radian( ss.second );

        _srcs.emplace_back(sncsphi(sincos(sin(theta), cos(theta)), phi));
    }
}

// Geometry of the system is such, that
// Z axis is going down,
// Y is going horizontally from left to right and
// X is whatever it is

// this code is to simulate conical source,
// to be removed and replaced by phase space file

// energy sampling, two monolines from Co60
static double sample_energy()
{
    if (G4UniformRand() < 0.5)
        return 1.33*MeV;

    return 1.17*MeV;
}

// sample source polar angle uniformly in the range
static double sample_polar(float polar_start, float polar_stop)
{
    double mu = polar_start + (polar_stop - polar_start)*G4UniformRand();
    return mu;
}

// generate particle at (0,0,0)
static std::tuple<double,double,double,double,double,double,double,double> generate_particle(double polar_start, double polar_stop)
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    auto cos_theta = sample_polar(polar_start, polar_stop);
    auto sin_theta = sqrt((1.0 - cos_theta) * (1.0 + cos_theta));
    auto phi       = 2.0 * M_PI * G4UniformRand();

    auto wx = cos_theta;
    auto wy = sin_theta*sin(phi);
    auto wz = sin_theta*cos(phi);

    auto e = sample_energy();
    auto w = 1.0;

    return std::make_tuple(w, e, x, y, z, wx, wy, wz);
}

static inline double sample_rotangle(double rstart, double rstop)
{
    return rstart + (rstop - rstart) * G4UniformRand();
}

static inline std::tuple<double, double> rotate_2d(double a, double o, double sn, double cs)
{
    return std::make_tuple(cs*a - sn*o, sn*a + cs*o);
}

// source particle parameters, called per each source event
void Source::GeneratePrimaries(G4Event* anEvent)
{
    double x, y, z;
    double wx, wy, wz;
    double w, e;

    // get generated at center but with proper direction
    std::tie(w, e, x, y, z, wx, wy, wz) = generate_particle(this->_polar_start, this->_polar_stop);

    // move source back in X, so it is proper
    // position
    x -= this->_iso_radius;

    // random collimator assembly rotation angle
    auto rndphi = sample_rotangle(_rot_start, _rot_stop);

    // now making it all together for all sources in the system
    for(decltype(_srcs.size()) k = 0; k != _srcs.size(); ++k) // running over all source
    {
        // per single collimator photon phs coordinates
        double xx, yy, zz;
        double wxx, wyy, wzz;

        xx = x;
        yy = y;
        zz = z;

        wxx = wx;
        wyy = wy;
        wzz = wz;

        // polar rotation, getting matrix
        auto sn = _srcs[k].first.first;
        auto cs = _srcs[k].first.second;

        // polar rotation, around Y axis, mixing X & Z and making proper latitude
        std::tie( zz, xx)  = rotate_2d( zz,  xx, sn, cs);
        std::tie(wzz, wxx) = rotate_2d(wzz, wxx, sn, cs);

        // aziumth rotation, around Z axis, making proper longitude

        // here is random position of the particular source
        auto phi = _srcs[k].second + rndphi; // source longitude plus rotation angle

        // compute aziumth rot.matrix
        sn = sin(phi);
        cs = cos(phi);

        // and rotate around Z, mixing X & Y
        std::tie(xx, yy)   = rotate_2d( xx,  yy, sn, cs);
        std::tie(wxx, wyy) = rotate_2d(wxx, wyy, sn, cs);

        // now add shift between phantom center and source isocenter
        _particleGun->SetParticlePosition(G4ThreeVector(xx + this->_shift_x,
                                                        yy + this->_shift_y,
                                                        zz + this->_shift_z));

        // set particle direction
        _particleGun->SetParticleMomentumDirection(G4ThreeVector(wxx, wyy, wzz));

        // and energy
        _particleGun->SetParticleEnergy(e);

        _particleGun->GeneratePrimaryVertex(anEvent);
    }
}
