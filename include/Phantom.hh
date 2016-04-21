#pragma once

#include <map>
#include <string>

#include "G4PhantomParameterisation.hh"

class G4VTouchable;
class G4VPhysicalVolume;
class G4Material;
class G4VisAttributes;

class Phantom : public G4PhantomParameterisation
{
#pragma region Data
    private: std::map<std::string, G4VisAttributes*> _colours;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Phantom();
    public: virtual ~Phantom() override;
#pragma endregion

#pragma region Interfaces
    public: virtual G4Material* ComputeMaterial(int rep_no, G4VPhysicalVolume* curVol,
                                                const G4VTouchable* parentTouch = nullptr) override;
#pragma endregion

    private: void read_colour_data();
};
