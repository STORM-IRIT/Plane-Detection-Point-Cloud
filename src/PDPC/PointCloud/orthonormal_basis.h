#pragma once

#include <PDPC/Common/Defines.h>

namespace pdpc {

//!
//! \brief compute an orthonormal basis {u,v,w} in 3D given w (normalized)
//!
inline void orthonormal_basis(Vector3& u, Vector3& v, const Vector3& w);
inline Matrix3 orthonormal_basis(const Vector3& w);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void orthonormal_basis(Vector3& u, Vector3& v, const Vector3& w)
{
    // i0: dimension where w extends the least
    int i0 = (std::abs(w[0] ) < std::abs(w[1])) ? 0  : 1;
        i0 = (std::abs(w[i0]) < std::abs(w[2])) ? i0 : 2;
    int i1 = (i0 + 1) % 3;
    int i2 = (i0 + 2) % 3;

    // setting the i0-th coordinate of u to zero ensure that u is not null
    u[i0] = 0;
    u[i1] =  w[i2];
    u[i2] = -w[i1];

    // normalize u
    u /= std::sqrt(u[i1] * u[i1] + u[i2] * u[i2]);

    v = w.cross(u);
}

Matrix3 orthonormal_basis(const Vector3& w)
{
    Matrix3 M;
    M.col(2) = w;

    // i0: dimension where w extends the least
    int i0 = (std::abs(w[0] ) < std::abs(w[1])) ? 0  : 1;
        i0 = (std::abs(w[i0]) < std::abs(w[2])) ? i0 : 2;
    int i1 = (i0 + 1) % 3;
    int i2 = (i0 + 2) % 3;

    // setting the i0-th coordinate of u to zero ensure that u is not null
    M.col(0)[i0] = 0;
    M.col(0)[i1] =  w[i2];
    M.col(0)[i2] = -w[i1];

    // normalize u
    M.col(0) /= std::sqrt(M.col(0)[i1] * M.col(0)[i1] + M.col(0)[i2] * M.col(0)[i2]);

    M.col(1) = w.cross(M.col(0));

    return M;
}

} // namespace pdpc
