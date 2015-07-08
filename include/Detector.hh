#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include <set>

class G4Material;
class G4Box;
class G4LogicalVolume;

class Detector : public G4VUserDetectorConstruction
{
#pragma region Data
    private: G4Material* _Air;
    private: G4Material* _Water;
    
    // World
    private: G4Box*             _world_box;
    private: G4LogicalVolume*   _world_lv;
    private: G4VPhysicalVolume* _world_pv;

    private: G4Box*             _container_solid;
    private: G4LogicalVolume*   _container_logic;
    private: G4VPhysicalVolume* _container_phys;

    private: std::vector<G4Material*> _materials;
    
    // list of new materials created to distinguish different density
    //  voxels that have the same original materials
    
    private: uint32_t* _matIDs; // index of material of each voxel

    private: int   _nofv_x;
    private: int   _nofv_y;
    private: int   _nofv_z;

    private: float _voxel_x;
    private: float _voxel_y;
    private: float _voxel_z;
    
    private: std::set<G4LogicalVolume*> _scorers;
    
    private: bool _constructed;
#pragma endregion
    
#pragma region Ctor/Dtor/ops
    public: Detector();
    public: ~Detector();
#pragma endregion

#pragma region Observers
    public: float voxel_x() const
    {
        return _voxel_x;
    }

    public: float voxel_y() const
    {
        return _voxel_y;
    }

    public: float voxel_z() const
    {
        return _voxel_z;
    }

    public: float voxel_volume() const
    {
        return _voxel_x*_voxel_y*_voxel_z;
    }

    public: int nofv_x() const
    {
        return _nofv_x;
    }

    public: int nofv_y() const
    {
        return _nofv_y;
    }

    public: int nofv_z() const
    {
        return _nofv_z;
    }

    public: int nof_voxels() const
    {
        return _nofv_x * _nofv_y * _nofv_z;
    }    
#pragma endregion

    public: virtual G4VPhysicalVolume* Construct() override;

    protected: void InitialisationOfMaterials();

    protected: void ConstructPhantomContainer();

    void SetScorer(G4LogicalVolume* voxel_logic);

    virtual void ConstructSDandField();
};

