#pragma once

#include "globals.hh"

class PhantomSetup
{
#pragma region Data
    private: int   _nofv_x;
    private: int   _nofv_y;
    private: int   _nofv_z;

    private: float _voxel_x;
    private: float _voxel_y;
    private: float _voxel_z;

    private: float _cube_x;
    private: float _cube_y;
    private: float _cube_z;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: PhantomSetup(const char *hed_name);

    public: PhantomSetup(const PhantomSetup& phs):
        _nofv_x{phs._nofv_x},
        _nofv_y{phs._nofv_y},
        _nofv_z{phs._nofv_z},

        _voxel_x{phs._voxel_x},
        _voxel_y{phs._voxel_y},
        _voxel_z{phs._voxel_z},

        _cube_x{phs._cube_x},
        _cube_y{phs._cube_y},
        _cube_z{phs._cube_z}
    {
    }

    public: PhantomSetup(PhantomSetup&& phs):
        _nofv_x{phs._nofv_x},
        _nofv_y{phs._nofv_y},
        _nofv_z{phs._nofv_z},

        _voxel_x{phs._voxel_x},
        _voxel_y{phs._voxel_y},
        _voxel_z{phs._voxel_z},

        _cube_x{phs._cube_x},
        _cube_y{phs._cube_y},
        _cube_z{phs._cube_z}
    {
    }

    public: PhantomSetup& operator=(const PhantomSetup& phs)
    {
        _nofv_x = phs._nofv_x;
        _nofv_y = phs._nofv_y;
        _nofv_z = phs._nofv_z;

        _voxel_x = phs._voxel_x;
        _voxel_y = phs._voxel_y;
        _voxel_z = phs._voxel_z;

        _cube_x = phs._cube_x;
        _cube_y = phs._cube_y;
        _cube_z = phs._cube_z;

        return *this;
    }

    public: PhantomSetup& operator=(PhantomSetup&& phs)
    {
        _nofv_x = phs._nofv_x;
        _nofv_y = phs._nofv_y;
        _nofv_z = phs._nofv_z;

        _voxel_x = phs._voxel_x;
        _voxel_y = phs._voxel_y;
        _voxel_z = phs._voxel_z;

        _cube_x = phs._cube_x;
        _cube_y = phs._cube_y;
        _cube_z = phs._cube_z;

        return *this;
    }

    public: ~PhantomSetup()
    {
    }
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

    public: float cube_x() const
    {
        return _cube_x;
    }

    public: float cube_y() const
    {
        return _cube_y;
    }

    public: float cube_z() const
    {
        return _cube_z;
    }

    // return linear index given 3 axial indices
    public: int idx(int ix, int iy, int iz) const
    {
        return ix + _nofv_x*(iy + iz*_nofv_y);
    }
#pragma endregion
};
