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
    private: std::map<string, G4VisAttributes*> _colours;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Phantom();
    public: ~Phantom();
#pragma endregion
  
    public:  G4Material* ComputeMaterial(int repNo, G4VPhysicalVolume* curVol,
                                         const G4VTouchable* parentTouch=nullptr) override;
  
    private: void read_colour_data();
};
