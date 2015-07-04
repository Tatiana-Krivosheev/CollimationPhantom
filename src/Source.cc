#include <cmath>

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
    _iso_radius{-1.0f}
{
    _particleGun = new G4ParticleGun( 1 );

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "gamma"; // "geantino"
    _particleGun->SetParticleDefinition(particleTable->FindParticle(particleName));

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

void Source::prepare()
{
    _srcs.reserve(_sources.size());
    for(auto k = 0; k != _sources.size(); ++k)
    {
        auto theta = _sources[k].first;
        _srcs.emplace_back(sncsphi(sincos(sin(theta), cos(theta)), _sources[k].second * M_PI/180.0f));
    }
}

inline float sample_angle()
{
    return 2.0f * float(M_PI * G4UniformRand());
}

inline std::tuple<float,float> rotate_2d(x, y, sn, cs)
{
    return std::make_tuple(cs*x - sn*y, sn*x + cs*y);
}

// source particle parameters, called per each source event
void Source::GeneratePrimaries(G4Event* anEvent)
{
    float x, y, z, wx, wy, wz, e, w;

    std::tie(x, y, z, wx, wy, wz, e, w) = read_phsf_record();
    
    // random collimator system rotation
    auto rndphi = sample_direction();

    for(int k = 0; k != _srcs.size(); ++k) // running over all source
    {
        z -= _iso_radius;        

        auto phi = _srcs[k].second + rndphi;
        
        auto cs_phi = cos(phi);
        auto sn_phi = sin(phi);
        
        // polar rotation
        std::tie(y, z) = rotate_2d(y, z, _srcs[k].first.first, _srcs[k].first.second);
        
        // aziumth rotation
        std::tie(x, y) = rotate_2d(x, y, cs_phi, sn_phi);

        _particleGun->SetParticlePosition(G4ThreeVector(x, y, z));

        _particleGun->SetParticleMomentumDirection(G4ThreeVector(wx, wy, wz));
       
        _particleGun->SetParticleEnergy(e);
        
        _particleGun->GeneratePrimaryVertex(anEvent);
    }
}

