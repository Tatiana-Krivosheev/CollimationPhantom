#include <fstream>

#include "Phantom.hh"

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
    
    int         nM;

    is >> nM;
    for(int k = 0; k != nM; ++k)
    {
        std::string mateName;
        double red, green, blue, opacity;

        is >> mateName >> red >> green >> blue >> opacity;
        G4VisAttributes* visAtt = new G4VisAttributes( G4Colour{ red, green, blue, opacity } );
        // visAtt->SetForceSolid(true);
        _colours[mateName] = visAtt;
    }    
}

G4Material* Phantom::ComputeMaterial(int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable*)
{
    G4Material* mats = G4PhantomParameterisation::ComputeMaterial( copyNo, physVol, nullptr );
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
            it = _colours.find("Default"); // default when there is no color name
        }
        
        physVol->GetLogicalVolume()->SetVisAttributes( it->second );
    }
    
    return mats;
}
