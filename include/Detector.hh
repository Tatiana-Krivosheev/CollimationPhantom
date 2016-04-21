#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "PhantomSetup.hh"

class G4Material;
class G4Box;
class G4LogicalVolume;

class Detector : public G4VUserDetectorConstruction
{
#pragma region Data
    private: G4Material* _Air;
    private: G4Material* _Water;

    // World
    private: G4Box*             _world_solid;
    private: G4LogicalVolume*   _world_logic;
    private: G4VPhysicalVolume* _world_phys;

    private: G4Box*             _container_solid;
    private: G4LogicalVolume*   _container_logic;
    private: G4VPhysicalVolume* _container_phys;

    // list of new materials created to distinguish different density
    // voxels that have the same original materials
    private: std::vector<G4Material*>   _materials;
    private: size_t*                    _mat_IDs; // index of material of each voxel

    private: PhantomSetup               _phs;

    private: std::set<G4LogicalVolume*> _scorers;

    private: bool                       _constructed;
    private: bool                       _checkOverlaps;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public:          Detector(const PhantomSetup& phs);
    public: virtual ~Detector();
#pragma endregion

#pragma region Observers
    public: float voxel_x() const
    {
        return _phs.voxel_x();
    }

    public: float voxel_y() const
    {
        return _phs.voxel_y();
    }

    public: float voxel_z() const
    {
        return _phs.voxel_z();
    }

    public: float voxel_volume() const
    {
        return _phs.voxel_volume();
    }

    public: float cube_x() const
    {
        return _phs.cube_x();
    }

    public: float cube_y() const
    {
        return _phs.cube_y();
    }

    public: float cube_z() const
    {
        return _phs.cube_z();
    }

    public: int nofv_x() const
    {
        return _phs.nofv_x();
    }

    public: int nofv_y() const
    {
        return _phs.nofv_y();
    }

    public: int nofv_z() const
    {
        return _phs.nofv_z();
    }

    public: int nof_voxels() const
    {
        return _phs.nof_voxels();
    }
#pragma endregion

    public: virtual G4VPhysicalVolume* Construct() override;

    public: virtual void ConstructSDandField() override;

    protected: void init_materials();

    protected: void make_phantom_container();

    protected: void make_phantom();

    public: void set_scorer(G4LogicalVolume* voxel_logic);
};
