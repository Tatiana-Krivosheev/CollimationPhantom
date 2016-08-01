#pragma once

#include <string>
#include <cmath>
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

    // isocentre radius, mm
    private: float               _iso_radius;

    // source collimator angle, radian
    private: float               _src_angle;

    // collimator sampling angles cosines
    private: float               _polar_start;
    private: float               _polar_stop;

    // assembly rotation range, radians
    private: float               _rot_start;
    private: float               _rot_stop;

    // phantom-to-source shift, mm, default values are NOT set
    private: float               _shift_x;
    private: float               _shift_y;
    private: float               _shift_z;

    // processed source info
    private: std::vector<sncsphi>  _srcs;

    private: G4ParticleDefinition* _gamma;
    private: G4ParticleDefinition* _electron;
    private: G4ParticleDefinition* _positron;
    private: G4ParticleDefinition* _geantino;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Source();
    public: Source(const Source& src) = delete;
    public: Source(Source&& src)      = default;

    public: Source&  operator=(const Source& src) = delete;
    public: Source&  operator=(Source&& src)      = default;

    public: virtual ~Source();
#pragma endregion

#pragma region Observers
    public: size_t nof_srcs() const
    {
        return _srcs.size();
    }

    public: float iso_radius() const
    {
        return _iso_radius;
    }

    public: float src_angle() const
    {
        return _src_angle;
    }

    public: float polar_start() const
    {
        return _polar_start;
    }

    public: float polar_stop() const
    {
        return _polar_stop;
    }

    public: float rot_start() const
    {
        return _rot_start;
    }

    public: float rot_stop() const
    {
        return _rot_stop;
    }

    public: float shift_x() const
    {
        return _shift_x;
    }

    public: float shift_y() const
    {
        return _shift_y;
    }

    public: float shift_z() const
    {
        return _shift_z;
    }

    public: std::vector<sncsphi> sources() const
    {
        return _srcs;
    }
#pragma endregion

#pragma region Mutators
    public: void GeneratePrimaries(G4Event* anEvent) override;

    public: void set_iso_radius(float radius)
    {
        _iso_radius = radius;
    }

    public: void set_src_angle(float angle);

    public: void set_rot_start(float angle)
    {
        _rot_start = angle;
    }

    public: void set_rot_stop(float angle)
    {
        _rot_stop = angle;
    }

    public: void set_shift_x(float shift)
    {
        _shift_x = shift;
    }

    public: void set_shift_y(float shift)
    {
        _shift_y = shift;
    }

    public: void set_shift_z(float shift)
    {
        _shift_z = shift;
    }

    public: void set_sources(const std::string& fname);

    private: void set_sources(const std::vector<angles>& srcs);
#pragma endregion
};
