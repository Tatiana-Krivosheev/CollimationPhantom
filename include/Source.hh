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
class G4ParticleDefinition;

class SourceMessenger;

class Source : public G4VUserPrimaryGeneratorAction
{
#pragma region Typedefs
    // source angles, <phi, theta>
    public: using angles  = std::pair<float, float>; // source position as pait of <latitude, longitude>
    public: using sincos  = std::pair<float, float>; // same sources, but position converted to trigs of angles
    public: using sncsphi = std::pair<sincos,float>; // all data for fast position description
#pragma endregion

#pragma region Data
    private: G4ParticleGun*      _particleGun;
    private: SourceMessenger*    _sourceMessenger;

    // number of collimator rows
    private: int                 _nof_rows;
    // number of collimator in a row
    private: int                 _nof_cols;

    // isocentre radius
    private: float               _iso_radius;

    // unprocessed data, as read from table,
    // angles are in degrees
    private: std::vector<angles>  _sources;

    // processed source info
    private: std::vector<sncsphi> _srcs;

    private: G4ParticleDefinition* _gamma;
    private: G4ParticleDefinition* _electron;
    private: G4ParticleDefinition* _positron;
    private: G4ParticleDefinition* _geantino;
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

    public: std::vector<angles> sources() const
    {
        return _sources;
    }
#pragma endregion

#pragma region Mutators
    public: void GeneratePrimaries(G4Event* anEvent);

    public: void set_nof_rows(int nrows)
    {
        _nof_rows = nrows;
    }

    public: void set_nof_cols(int ncols)
    {
        _nof_cols = ncols;
    }

    public: void set_iso_radius(float radius)
    {
        _iso_radius = radius;
    }

    public: void set_sources(const std::vector<angles>& srcs)
    {
        _sources.clear();
        _sources.reserve(srcs.size());
        _sources = srcs;
        prepare_sources();
    }

    public: void prepare_sources();
#pragma endregion
};

