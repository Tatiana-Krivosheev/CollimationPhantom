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
    _nof_particles{36}
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

double Source::sample_direction()
{
    double phi = 2.0 * M_PI * G4UniformRand();
    return phi; // pair<double,double>(sin(phi), cos(phi))
}

// source particle parameters, called per each source event
void Source::GeneratePrimaries(G4Event* anEvent)
{
    if (! _is.good())
        throw std::runtime_error();
        
    read_one_record();
    
    phi = sample_direction();

    for(int k = 0; k != 6; ++k) // number of collimation rows
    {
        // here we sample spatial decay vertex uniformly in the cylinder
        double z   = _halfz * ( 2.0*G4UniformRand() - 1.0 );
        double phi = 2.0 * M_PI * G4UniformRand();
        double r   = _radius * sqrt(G4UniformRand());

        auto x = r * cos(phi);
        auto y = r * sin(phi);
        _particleGun->SetParticlePosition(G4ThreeVector(x, y, z));

        auto dir = sample_direction();
        _particleGun->SetParticleMomentumDirection(G4ThreeVector(dir._wx, dir._wy, dir._wz));
       
        auto e = sample_energy();
        _particleGun->SetParticleEnergy(e);
        
        _particleGun->GeneratePrimaryVertex(anEvent);
    }
}

