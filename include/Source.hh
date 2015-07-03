#pragma once

#include <string>
#include <fstream>

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;
class SourceMessenger;

class Source : public G4VUserPrimaryGeneratorAction
{
#pragma region Data
    private: G4ParticleGun*      _particleGun;
    private: SourceMessenger*    _sourceMessenger;

    private: std::string         _fname;
    private: std::ifstream       _is;
        
    private: int                 _nof_particles; 
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Source();
    public: ~Source();
#pragma endregion

#pragma region Observers
    public: std::string fname() const
    {
        return _fname;
    }

    public: std::istream& is() const
    {
        return _is;
    }

    public: int nof_particles() const
    {
        return _nof_particles;
    }
#pragma endregion

    public: static double sample_angle();

#pragma region Mutators
    public: void GeneratePrimaries(G4Event* anEvent);

    public: void set_fname(const std::string& fname)
    {
        _fname = fname;
        _is.close();
        _is.open(fname);
    }
#pragma endregion
};

