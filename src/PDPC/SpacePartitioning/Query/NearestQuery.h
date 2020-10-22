#pragma once

#include <PDPC/Common/Defines.h>

#include <cmath>

namespace pdpc {

class NearestQuery
{
public:
    NearestQuery(){}

    int get() const{return m_nearest;}
    Scalar distance() const {return std::sqrt(m_squared_distance);}

protected:
    int m_nearest;
    Scalar m_squared_distance;
};

} // namespace pdpc
