#include <fstream>

#include "Phantom.hh"

#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

Phantom::Phantom():
    G4PhantomParameterisation{},
    _colours{}
{
    read_colour_data();
}

Phantom::~Phantom()
{
}

void Phantom::read_colour_data()
{
    G4VisAttributes* blank = new G4VisAttributes();
    
    blank->SetVisibility( FALSE );
    _colours["Default"] = blank;
    
    std::string colourFile = "ColorMap.dat";
    std::ifstream is(colourFile.c_str());
    
    int        nMate;
    std::tring mateName;
    double cred, cgreen, cblue, copacity;
    
    is >> nMate;
    for(int k = 0; k != nMate; ++k)
    {
        is >> mateName >> cred >> cgreen >> cblue >> copacity;
        G4Colour colour( cred, cgreen, cblue, copacity );
        G4VisAttributes* visAtt = new G4VisAttributes( colour ); // visAtt->SetForceSolid(true);
        _colours[mateName] = visAtt;
    }    
}

G4Material* Phantom::ComputeMaterial(int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable*)
{
    G4Material* mats = G4PhantomParameterisation::ComputeMaterial( copyNo, physVol, 0 );
    if( physVol )
    {
        std::string mat_name       = mats->GetName();
        std::string::size_type iuu = mat_name.find("__");
        if( iuu != std::string::npos )
        {
            mat_name = mat_name.substr( 0, iuu );
        }
        
        auto it = _colours.find(mat_name);

        if( it == _colours.end() )
        {
            it = _colours.begin(); // default when there is no color name
        }
        
        physVol->GetLogicalVolume()->SetVisAttributes( it->second );
    }
    
    return mats;
}

