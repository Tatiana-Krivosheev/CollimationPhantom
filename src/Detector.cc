#include "globals.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
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

#include "Detector.hh"

//#include "DicomRunAction.hh"
//#include "DicomRun.hh"


Detector::Detector():
    G4VUserDetectorConstruction(),
    _Air{nullptr},
    _Water{nullptr},

    _world_solid{nullptr},
    _world_logic{nullptr},
    _world_phys{nullptr},

    _container_solid{nullptr},
    _container_logic{nullptr},
    _container_phys{nullptr},
    
    _materials{nullptr}

   _matIDs{nullptr},

    _nofv_x{-1},
    _nofv_y{-1},
    _nofv_z{-1},

    _voxel_x{-1.0f},
    _voxel_y{-1.0f},
    _voxel_z{-1.0f},
    
    _constructed{false}
{
}

Detector::~Detector()
{
}

G4VPhysicalVolume* Detector::Construct()
{
    if( !_constructed || _world_phys == nullptr)
    {
        InitialisationOfMaterials();

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
                                         _checkOverlaps);
                                         
        ReadPhantomData();

        ConstructPhantomContainer();
        ConstructPhantom();
        _constructed = true;
    }
    return _world_phys;
}

void Detector::InitialisationOfMaterials()
{
    // Creating elements :
    double z, a, density;
    std::string name, symbol;

    G4Element* elC = new G4Element( name = "Carbon",
                                   symbol = "C",
                                   z = 6.0, a = 12.011 * g/mole );
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
    auto _Air = new G4Material( "Air",
                                1.290*mg/cm3,
                                numberofElements = 2 );
    _Air->AddElement(elN, 0.7);
    _Air->AddElement(elO, 0.3);


    // Water
    auto _Water = new G4Material( "Water",
                                  density = 1.0*g/cm3,
                                  numberofElements = 2 );
    _Water->AddElement(elH, 0.112);
    _Water->AddElement(elO, 0.888);

    //----- Put the materials in a vector
    _materials.push_back(fAir); // rho = 0.00129
    _materials.push_back(water); // rho = 1.018
}


void Detector::ReadPhantomData()
{

    G4String dataFile = "Data.dat";
    std::ifstream finDF(dataFile.c_str());
    G4String fname;
    if(finDF.good() != 1 ) {
        G4String descript = "Problem reading data file: "+dataFile;
        G4Exception(" DicomDetectorConstruction::ReadPhantomData",
                    "",
                    FatalException,
                    descript);
    }

    G4int compression;
    finDF >> compression; // not used here

    finDF >> fNoFiles;
    for(G4int i = 0; i < fNoFiles; i++ ) {
        finDF >> fname;
        //--- Read one data file
        fname += ".g4dcm";
        ReadPhantomDataFile(fname);
    }

    //----- Merge data headers
    MergeZSliceHeaders();

    finDF.close();

}

void Detector::ReadPhantomDataFile(const G4String& fname)
{
#ifdef G4VERBOSE
  G4cout << " DicomDetectorConstruction::ReadPhantomDataFile opening file " 
         << fname << G4endl;
#endif
  std::ifstream fin(fname.c_str(), std::ios_base::in);
  if( !fin.is_open() ) {
    G4Exception("DicomDetectorConstruction::ReadPhantomDataFile",
                "",
                FatalErrorInArgument,
                G4String("File not found " + fname ).c_str());
  }
  //----- Define density differences (maximum density difference to create
  // a new material)
  char* part = getenv( "DICOM_CHANGE_MATERIAL_DENSITY" );
  G4double densityDiff = -1.;
  if( part ) densityDiff = G4UIcommand::ConvertToDouble(part);
  if( densityDiff != -1. ) {
    for( unsigned int ii = 0; ii < fOriginalMaterials.size(); ii++ ){
      fDensityDiffs[ii] = densityDiff; //currently all materials with 
      // same difference
    }
  }else {
    if( fMaterials.size() == 0 ) { // do it only for first slice
      for( unsigned int ii = 0; ii < fOriginalMaterials.size(); ii++ ){
        fMaterials.push_back( fOriginalMaterials[ii] );
      }
    }
  }
  
  //----- Read data header
  DicomPhantomZSliceHeader* sliceHeader = new DicomPhantomZSliceHeader( fin );
  fZSliceHeaders.push_back( sliceHeader );
  
  //----- Read material indices
  G4int nVoxels = sliceHeader->GetNoVoxels();
  
  //--- If first slice, initiliaze fMateIDs
  if( fZSliceHeaders.size() == 1 ) {
    //fMateIDs = new unsigned int[fNoFiles*nVoxels];
    fMateIDs = new size_t[fNoFiles*nVoxels];
    
  }
  
  unsigned int mateID;
  // number of voxels from previously read slices
  G4int voxelCopyNo = (fZSliceHeaders.size()-1)*nVoxels;
  for( G4int ii = 0; ii < nVoxels; ii++, voxelCopyNo++ ){
    fin >> mateID;
    fMateIDs[voxelCopyNo] = mateID;
  }
  
  //----- Read material densities and build new materials if two voxels have
  //  same material but its density is in a different density interval 
  // (size of density intervals defined by densityDiff)
  G4double density;
  // number of voxels from previously read slices
  voxelCopyNo = (fZSliceHeaders.size()-1)*nVoxels;
  for( G4int ii = 0; ii < nVoxels; ii++, voxelCopyNo++ ){
    fin >> density;
    
    //-- Get material from list of original materials
    mateID = fMateIDs[voxelCopyNo];
    G4Material* mateOrig  = fOriginalMaterials[mateID];
    
    //-- Get density bin: middle point of the bin in which the current
    // density is included
    G4String newMateName = mateOrig->GetName();
    float densityBin = 0.;
    if( densityDiff != -1.) {
      densityBin = fDensityDiffs[mateID] * 
        (G4int(density/fDensityDiffs[mateID])+0.5);
      //-- Build the new material name
      newMateName += G4UIcommand::ConvertToString(densityBin);
    }
    
    //-- Look if a material with this name is already created
    //  (because a previous voxel was already in this density bin)
    unsigned int im;
    for( im = 0; im < fMaterials.size(); im++ ){
      if( fMaterials[im]->GetName() == newMateName ) {
        break;
      }
    }
    //-- If material is already created use index of this material
    if( im != fMaterials.size() ) {
      fMateIDs[voxelCopyNo] = im;
      //-- else, create the material
    } else {
      if( densityDiff != -1.) {
        fMaterials.push_back( BuildMaterialWithChangingDensity( mateOrig,
                                                  densityBin, newMateName ) );
        fMateIDs[voxelCopyNo] = fMaterials.size()-1;
      } else {
        G4cerr << " im " << im << " < " << fMaterials.size() << " name "
               << newMateName << G4endl;
        G4Exception("DicomDetectorConstruction::ReadPhantomDataFile",
                    "",
                    FatalErrorInArgument,
                    "Wrong index in material"); //it should never reach here
      }
    }
  }
  
}

G4Material* Detector::BuildMaterialWithChangingDensity(const G4Material* origMate, float density, G4String newMateName)
{
  //----- Copy original material, but with new density
  G4int nelem = origMate->GetNumberOfElements();
  G4Material* mate = new G4Material( newMateName, density*g/cm3, nelem,
                                     kStateUndefined, STP_Temperature );
  
  for( G4int ii = 0; ii < nelem; ii++ ){
    G4double frac = origMate->GetFractionVector()[ii];
    G4Element* elem = const_cast<G4Element*>(origMate->GetElement(ii));
    mate->AddElement( elem, frac );
  }
  
  return mate;
}

void Detector::ConstructPhantomContainer()
{
    fNVoxelX = fZSliceHeaderMerged->GetNoVoxelX();
    fNVoxelY = fZSliceHeaderMerged->GetNoVoxelY();
    fNVoxelZ = fZSliceHeaderMerged->GetNoVoxelZ();
  
  fVoxelHalfDimX = fZSliceHeaderMerged->GetVoxelHalfX();
  fVoxelHalfDimY = fZSliceHeaderMerged->GetVoxelHalfY();
  fVoxelHalfDimZ = fZSliceHeaderMerged->GetVoxelHalfZ();
#ifdef G4VERBOSE
  G4cout << " fNVoxelX " << fNVoxelX << " fVoxelHalfDimX " << fVoxelHalfDimX 
         <<G4endl;
  G4cout << " fNVoxelY " << fNVoxelY << " fVoxelHalfDimY " << fVoxelHalfDimY 
         <<G4endl;
  G4cout << " fNVoxelZ " << fNVoxelZ << " fVoxelHalfDimZ " << fVoxelHalfDimZ 
         <<G4endl;
  G4cout << " totalPixels " << fNVoxelX*fNVoxelY*fNVoxelZ <<  G4endl;
#endif
  
  //----- Define the volume that contains all the voxels
  fContainer_solid = new G4Box("phantomContainer",fNVoxelX*fVoxelHalfDimX,
                               fNVoxelY*fVoxelHalfDimY,
                               fNVoxelZ*fVoxelHalfDimZ);
  fContainer_logic =
    new G4LogicalVolume( fContainer_solid,
   //the material is not important, it will be fully filled by the voxels
                         fMaterials[0],
                         "phantomContainer",
                         0, 0, 0 );
  //--- Place it on the world
  G4double fOffsetX = (fZSliceHeaderMerged->GetMaxX() + 
                       fZSliceHeaderMerged->GetMinX() ) /2.;
  G4double fOffsetY = (fZSliceHeaderMerged->GetMaxY() + 
                       fZSliceHeaderMerged->GetMinY() ) /2.;
  G4double fOffsetZ = (fZSliceHeaderMerged->GetMaxZ() + 
                       fZSliceHeaderMerged->GetMinZ() ) /2.;
  G4ThreeVector posCentreVoxels(fOffsetX,fOffsetY,fOffsetZ);
#ifdef G4VERBOSE
  G4cout << " placing voxel container volume at " << posCentreVoxels << G4endl;
#endif_nofv_x
  fContainer_phys =
    new G4PVPlacement(0,  // rotation
                      posCentreVoxels,
                      fContainer_logic,     // The logic volume
                      "phantomContainer",  // Name
                      fWorld_logic,  // Mother
                      false,           // No op. bool.
                      1);              // Copy number
  
  //fContainer_logic->SetVisAttributes(new G4VisAttributes(G4Colour(1.,0.,0.)));
}

void Detector::SetScorer(G4LogicalVolume* voxel_logic)
{
    std::cout << "\n\n\n\n\t SET SCORER : " << _voxel_logic->GetName() 
              << " \n\n\n" << G4endl;
  
    _scorers.insert(_voxel_logic);
}

void Detector::ConstructSDandField()
{
    std::cout << "\n\n\n\n\t CONSTRUCT SD AND FIELD \n\n\n" << G4endl;
  
    // Sensitive Detector Name
    std::string concreteSDname = "phantomSD";
    std::vector<std::tring> scorer_names;
    scorer_names.push_back(concreteSDname);
    
    //------------------------
    // MultiFunctionalDetector
    //------------------------
    
    // Define MultiFunctionalDetector with name.
    // declare MFDet as a MultiFunctionalDetector scorer
    G4MultiFunctionalDetector* MFDet = new G4MultiFunctionalDetector(concreteSDname);
    
    G4VPrimitiveScorer* dosedep = new G4PSDoseDeposit3D("DoseDeposit", _nofv_x, _nofv_y, _nofv_z);
    MFDet->RegisterPrimitive(dosedep);
  
    for(auto ite = scorers.begin(); ite != scorers.end(); ++ite)
    {
        SetSensitiveDetector(*ite, MFDet);
    }
}


