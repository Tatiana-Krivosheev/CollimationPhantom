#pragma once

#include <string>
#include <fstream>
#include <utility>
#include <vector>

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

    // number of collimator rows
    private: int                 _nof_rows;
    // number of collimators in a row
    private: int                 _nof_cols;

    // isocentre radius
    private: float               _iso_radius;
    
    // source angles, <phi, theta>
    private: using angles = std::pair<float,float>;
    private: using sincos = std::pair<float,float>;
    private: using sncsphi = std::pair<sincos,float>;    
    
    private: std::vector<angles> _sources;
    
    // processed source info
    private: std::vector<sncsphi> _srcs;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Source();
    public: ~Source();
#pragma endregion

#pragma region Observers
    public: int nof_rows() const
    {
        return _nof_rows;
    }
    
    public: int nof_cols() const
    {
        return _nof_cols;
    }
    
    public: float iso_radius() const
    {
        return _iso_radius;
    }
#pragma endregion

#pragma region Mutators
    public: void GeneratePrimaries(G4Event* anEvent);

    public: void set_nof_rows(int nof_rows)
    {
        _nof_rows = nof_rows;
    }
    
    public: void set_nof_cols(int nof_cols)
    {
        _nof_cols = nof_cols;
    }
    
    public: void set_iso_radius(float radius)
    {
        _iso_radius = radius;
    }
#pragma endregion
};

