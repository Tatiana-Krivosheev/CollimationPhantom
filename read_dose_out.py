#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt

def read_phantom_head(fname):
    """
    Given filename, read and parse phantom header file

    return voxels and dimensions
    """

    lines = None
    with open(fname) as f:
        lines = f.readlines()

    vx = 0.0
    vy = 0.0
    vz = 0.0

    nx = 0
    ny = 0
    nz = 0

    for line in lines:
        s = line.split(" ")
        s = [x for x in s if x]  # remove empty lines

        if s[0] == "VOXELSIZE":
            vx = float(s[2])
            vy = float(s[3])
            vz = float(s[4])
            continue

        if s[0] == "DIMENSION":
            nx = int(s[2])
            ny = int(s[3])
            nz = int(s[4])
            continue

        if s[0] == "END-INPUT":
            break

    return (vx, vy, vz, nx, ny, nz)

def read_dose_out(fname):
    """
    Read and parse dose.out file

    Return dictionary of dose@idx
    """

    lines = None
    with open(fname) as f:
        lines = f.readlines()

    dout = {}
    for line in lines:
        s = line.split(" ")
        s = [x for x in s if x]  # remove empty lines

        if len(s) >= 2:
            idx  = int(s[0])
            dose = float(s[1])

            dout[idx] = dose

    return dout

def make_dose_array(nx, ny, nz, dout):
    """
    Convert dose dictionary into 3D dose array
    """

    # allocate 3D array
    dose = np.zeros((nx, ny, nz), dtype=np.float32) # all zeros by default

    for iz in range(0, nz):
        for iy in range(0, ny):
            for ix in range(0, nx):
                idx  = ix + iy*nx + iz*nx*ny
                idx += nz

                dose[ix, iy, iz] = dout[idx]

    return dose

def make_z_axis(nz, vz):
    """
    nz - number of voxels
    vz - voxel size

    returns Z axis in cup CS
    """

    z = np.zeros(nz, dtype=np.float32)
    start = 0.0
    for k in range(0, nz):
        z[k] = start + ( float(k) + 0.5 ) * vz

    return z

def make_xy_axis(n, v):
    """
    n - number of voxels
    v - voxel size

    returns X or Y axis in cup CS
    """

    xy = np.zeros(n, dtype=np.float32)
    start = -float(n/2)*v
    for k in range(0, n):
        xy[k] = start + ( float(k) + 0.5 ) * v

    return xy

if __name__ == "__main__":

    vx, vy, vz, nx, ny, nz = read_phantom_head("phantom.hed")
    print(vx, vy, vz, nx, ny, nz)

    dout = read_dose_out("dose.out")
    print(len(dout))

    dose = make_dose_array(nx, ny, nz, dout)
    print(dose.shape)
    #print(dose)

    ix = nx//2 - 1 # we start with 0 for X
    iy = ny//2 - 1 # we start with 0 for Y
    iz = nz//2 - 1 # we start with 0 for Z

    dz = 1.0e+7*(dose[ix, iy, :] + dose[ix+1, iy, :] + dose[ix, iy+1, :] + dose[ix+1, iy+1, :])/4.0
    z  = make_z_axis(nz, vz)

    dx = 1.0e+7*(dose[:, iy, iz] + dose[:, iy, iz+1] + dose[:, iy+1, iz] + dose[:, iy+1, iz+1])/4.0
    x = make_xy_axis(nx, vx)

    dy = 1.0e+7*(dose[ix, :, iz] + dose[ix, :, iz+1] + dose[ix+1, :, iz] + dose[ix+1, :, iz+1])/4.0
    y = make_xy_axis(ny, vy)

    #plt.scatter(z, dz, alpha=0.1)
    fig = plt.figure()
    fig.suptitle('Dose distribution', fontsize=14, fontweight='bold')

    ax = fig.add_subplot(111)
    fig.subplots_adjust(top=0.85)
    ax.set_title('Z profile')

    ax.set_xlabel('Distance, mm')
    ax.set_ylabel('Dose, a.u.')

    ax.plot(z, dz, '*')
    #ax.axis([0, 10, 0, 10])
    #plt.scatter(y, dy, alpha=0.1)

    plt.show()
