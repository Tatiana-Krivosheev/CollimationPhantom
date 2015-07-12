#include <cmath>
#include <tuple>

#include "Source.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "SourceMessenger.hh"
#include "globals.hh"

#include "Randomize.hh"

// here you set global source parameters, called once per run
Source::Source():
    _particleGun{nullptr},
    _sourceMessenger{nullptr},
    _nof_rows{-1},
    _nof_cols{-1},
    _iso_radius{-1.0f},
    _photon{nullptr},
    _electron{nullptr},
    _positron{nullptr},
    _geantino{nullptr}
{
    _particleGun = new G4ParticleGun( 1 );

    auto particleTable = G4ParticleTable::GetParticleTable();
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

void Source::prepare_sources()
{
    _srcs.clear();
    _srcs.reserve(_sources.size());
    for(auto k = 0; k != _sources.size(); ++k)
    {
        auto theta = _sources[k].first  * M_PI/180.0f;
        auto phi   = _sources[k].second * M_PI/180.0f;
        
        _srcs.emplace_back(sncsphi(sincos(sin(theta), cos(theta)), phi));
    }
}

// this code is to simulate conical source,
// to be removed and replaced by phase space file
static double sample_energy()
{
    if (G4UniformRand() < 0.5)
        return 1.33*MeV;
        
    return 1.17*MeV;
}

static double sample_polar()
{
    static double polar_start = 0.9994;
    static double polar_end   = 1.0000;

    mu = polar_start + (polar_end - polar_start)*G4UniformRand();
}

static std::tuple<double,double,double,double,double,double,double,double> generate_particle()
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    auto cos_theta = sample_polar();
    auto sin_theta = sqrt((1.0 - cos_theta)*(1.0 + cos_theta));
    auto phi       = 2.0 * M_PI * G4UniformRand();
    
    auto wx = sin_theta*cos(phi);
    auto wy = sin_theta*sin(phi);
    auto wz = cos_theta;
    
    auto e = sample_energy();
    auto w = 1.0;
    
    return std::make_tuple(w, e, x, y, z, wx, wy, wz);
}
// end-of-source

inline double sample_rotangle()
{
    return 2.0f * M_PI * G4UniformRand();
}

inline std::tuple<float,float> rotate_2d(double a, double o, double sn, double cs)
{
    return std::make_tuple(cs*a - sn*o, sn*a + cs*o);
}

// source particle parameters, called per each source event
void Source::GeneratePrimaries(G4Event* anEvent)
{
    double x, y, z;
    double wx, wy, wz;
    double w, e;
    
    std::tie(w, e, x, y, z, wx, wy, wz) = generate_particle();
    
    // random collimator system rotation
    auto rndphi = sample_rotangle();
    
    for(int k = 0; k != _srcs.size(); ++k) // running over all source
    {
        double xx, yy, zz;
        double wxx, wyy, wzz;
        
        zz = z + _iso_radius; // move point forward

        auto sn = _srcs[k].first.first
        auto cs = _srcs[k].first.second;
        
        auto phi = _srcs[k].second + rndphi;        

        // polar rotation, around X axis
        std::tie( zz, yy)  = rotate_2d( z,  y, sn, cs);
        std::tie(wzz, wyy) = rotate_2d(wz, wy, sn, cs);
        
        // aziumth rotation, around Z axis
        sn = sin(phi);
        cs = cos(phi);
        std::tie(xx, yy) = rotate_2d( x, yy, sn, cs);
        std::tie(wxx, wyy) = rotate_2d(wx, wyy, sn, cs);

        _particleGun->SetParticlePosition(G4ThreeVector(xx, yy, zz));

        // reflect direction
        _particleGun->SetParticleMomentumDirection(G4ThreeVector(-wxx, -wyy, -wzz));
       
        _particleGun->SetParticleEnergy(e);
        
        _particleGun->GeneratePrimaryVertex(anEvent);
    }
}

