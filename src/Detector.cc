#include "globals.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UIcommand.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"

#include "PhantomSetup.hh"
#include "Phantom.hh"
#include "Detector.hh"

Detector::Detector(const PhantomSetup& phs):
    G4VUserDetectorConstruction{},
    _Air{nullptr},
    _Water{nullptr},

    _world_solid{nullptr},
    _world_logic{nullptr},
    _world_phys{nullptr},

    _container_solid{nullptr},
    _container_logic{nullptr},
    _container_phys{nullptr},

    _materials{},

    _mat_IDs{nullptr},

    _phs{phs},

    _scorers{},

    _constructed{false},

    _checkOverlaps{true}
{
}

Detector::~Detector()
{
}

G4VPhysicalVolume* Detector::Construct()
{
    if( !_constructed || _world_phys == nullptr)
    {
        init_materials();

        //----- Build world
        double world_x = 50.0 * cm;
        double world_y = 50.0 * cm;
        double world_z = 50.0 * cm;

        _world_solid = new G4Box( "WorldSolid", world_x, world_y, world_z );

        _world_logic = new G4LogicalVolume( _world_solid,
                                            _Air,
                                            "WorldLogical",
                                            nullptr, nullptr, nullptr );

        _world_phys = new G4PVPlacement( nullptr,              // no rotation
                                         G4ThreeVector(0,0,0), // no translation
                                         _world_logic,         // its logical volume
                                         "World",              // its name
                                         nullptr,              // no mother volume
                                         false,                // no boolean ops
                                         0,                    // copy number
                                         _checkOverlaps );

        make_phantom_container();
        make_phantom();

        _constructed = true;
    }

    return _world_phys;
}

void Detector::init_materials()
{
    // Creating elements :
    double z, a, density;
    std::string name, symbol;

//    G4Element* elC = new G4Element( name = "Carbon",
//                                    symbol = "C",
//                                    z = 6.0, a = 12.011 * g/mole );
    G4Element* elH = new G4Element( name = "Hydrogen",
                                    symbol = "H",
                                    z = 1.0, a = 1.008  * g/mole );
    G4Element* elN = new G4Element( name = "Nitrogen",
                                    symbol = "N",
                                    z = 7.0, a = 14.007 * g/mole );
    G4Element* elO = new G4Element( name = "Oxygen",
                                    symbol = "O",
                                    z = 8.0, a = 16.00  * g/mole );

    int numberofElements = -1;
    // Air
    _Air = new G4Material( "Air",
                           1.290*mg/cm3,
                           numberofElements = 2 );
    _Air->AddElement(elN, 0.7);
    _Air->AddElement(elO, 0.3);

    // Water
    _Water = new G4Material( "Water",
                             density = 1.0*g/cm3,
                             numberofElements = 2 );
    _Water->AddElement(elH, 0.112);
    _Water->AddElement(elO, 0.888);

    //----- Put the materials in a vector
    _materials.push_back(_Air);   // rho = 0.00129
    _materials.push_back(_Water); // rho = 1.018
}

void Detector::make_phantom_container()
{
    // Define the volume that contains all the voxels
    _container_solid = new G4Box{"phantomContainer",
                                  0.5*cube_x(),
                                  0.5*cube_y(),
                                  0.5*cube_z() };

    _container_logic = new G4LogicalVolume{ _container_solid,
                                            _Air, // material is not important, it will be fully filled by the voxels
                                           "phantomContainer",
                                           nullptr,   // field
                                           nullptr,   // sensitive detector
                                           nullptr }; // user limits

    // Place it on the world
    double offset_x = 0.0;
    double offset_y = 0.0;
    double offset_z = 0.0;

    G4ThreeVector pos_center_voxels( offset_x, offset_y, offset_z );

    _container_phys = new G4PVPlacement( nullptr,             // rotation
                                         pos_center_voxels,   // translation
                                         _container_logic,    // The logic volume
                                         "phantomContainer",  // name
                                         _world_logic,        // mother
                                         false,               // No op. bool.
                                         1 );                 // copy number
}

void Detector::set_scorer(G4LogicalVolume* voxel_logic)
{
    _scorers.insert(voxel_logic);
}

void Detector::ConstructSDandField()
{
    // Sensitive Detector Name
    std::string concreteSDname{"phantomSD"};
    std::vector<std::string> scorer_names;
    scorer_names.push_back(concreteSDname);

    //------------------------
    // MultiFunctionalDetector
    //------------------------

    // Define MultiFunctionalDetector with name.
    // declare MFDet as a MultiFunctionalDetector scorer
    G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(concreteSDname);

    G4VPrimitiveScorer* dosedep = new G4PSDoseDeposit3D("DoseDeposit", _phs.nofv_x(), _phs.nofv_y(), _phs.nofv_z());
    MFDet->RegisterPrimitive(dosedep);

    for(auto ite = _scorers.begin(); ite != _scorers.end(); ++ite)
    {
        SetSensitiveDetector(*ite, MFDet);
    }
}

void Detector::make_phantom()
{
    //----- Create parameterisation
    Phantom* phantom = new Phantom();

    //----- Set voxel dimensions
    phantom->SetVoxelDimensions( 0.5 * _phs.voxel_x(), 0.5 * _phs.voxel_y(), 0.5 * _phs.voxel_z() );

    //----- Set number of voxels
    phantom->SetNoVoxel( _phs.nofv_x(), _phs.nofv_y(), _phs.nofv_z() );

    //----- Set list of materials
    phantom->SetMaterials( _materials );

    //----- Set list of material indices: for each voxel it is a number that
    // correspond to the index of its material in the vector of materials
    // defined above
    phantom->SetMaterialIndices( _mat_IDs );

    //----- Define voxel logical volume
    G4Box* voxel_solid = new G4Box( "Voxel",
                                    0.5 * _phs.voxel_x(), 0.5 * _phs.voxel_y(), 0.5 * _phs.voxel_z());
    G4LogicalVolume* voxel_logic =  new G4LogicalVolume(voxel_solid,
                                                        _materials[0],
                                                        "VoxelLogical",
                                                        0,0,0);
    // material is not relevant, it will be changed by the
    // ComputeMaterial method of the parameterisation

    voxel_logic->SetVisAttributes(new G4VisAttributes(G4VisAttributes::Invisible));

    //--- Assign the fContainer volume of the parameterisation
    phantom->BuildContainerSolid(_container_phys);

    //--- Assure yourself that the voxels are completely filling the
    // fContainer volume
    phantom->CheckVoxelsFillContainer( _container_solid->GetXHalfLength(),
                                       _container_solid->GetYHalfLength(),
                                       _container_solid->GetZHalfLength() );


    //----- The G4PVParameterised object that uses the created parameterisation
    // should be placed in the fContainer logical volume
    G4PVParameterised * phantom_phys = new G4PVParameterised("phantom", voxel_logic, _container_logic,
                                                              kXAxis,
                                                              _phs.nofv_x() * _phs.nofv_y() * _phs.nofv_z(),
                                                              phantom);
    // if axis is set as kUndefined instead of kXAxis, GEANT4 will
    //  do an smart voxel optimisation
    // (not needed if G4RegularNavigation is used)

    //----- Set this physical volume as having a regular structure of type 1,
    // so that G4RegularNavigation is used
    phantom_phys->SetRegularStructureId(1); // if not set, G4VoxelNavigation
    //will be used instead

    set_scorer(voxel_logic);
}
