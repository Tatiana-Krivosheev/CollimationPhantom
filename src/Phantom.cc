#include <fstream>

#include "Phantom.hh"

#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

Phantom::Phantom():
    G4PhantomParameterisation()
{
    read_colour_data();
    SetSkipEqualMaterials(false);
}

Phantom::~Phantom()
{
}

void Phantom::read_colour_data()
{
    G4VisAttributes* blankAtt = new G4VisAttributes();
    
    blankAtt->SetVisibility( FALSE );
    _colours["Default"] = blankAtt;
    
    std::string colourFile = "ColourMap.dat";
    std::ifstream is(colourFile.c_str());
    
    int        nMate;
    std::tring mateName;
    double cred, cgreen, cblue, copacity;
    
    is >> nMate;
    for(int k = 0; k != nMate; ++k)
    {
        is >> mateName >> cred >> cgreen >> cblue >> copacity;
        G4Colour colour( cred, cgreen, cblue, copacity );
        G4VisAttributes* visAtt = new G4VisAttributes( colour );
        //visAtt->SetForceSolid(true);
        _colours[mateName] = visAtt;
    }    
}

G4Material* Phantom::ComputeMaterial(int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable*)
{
    G4Material* mate = G4PhantomParameterisation::ComputeMaterial( copyNo, physVol, 0 );
    if( physVol )
    {
        std::string mateName = mate->GetName();
        std::string::size_type iuu = mateName.find("__");
        if( iuu != std::string::npos )
        {
            mateName = mateName.substr( 0, iuu );
        }
        
        auto it = _colours.find(mateName);

        if( it == _colours.end() )
        {
            it = _colours.begin(); // default when there is no color name
        }
        
        physVol->GetLogicalVolume()->SetVisAttributes( it->second );
    }
    
    return mate;
}

