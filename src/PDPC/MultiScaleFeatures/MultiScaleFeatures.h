#pragma once

#include <PDPC/Common/Defines.h>
#include <PDPC/Common/Assert.h>

namespace pdpc {

class MultiScaleFeatures
{
public:
    MultiScaleFeatures(int point_count = 0, int scale_count = 0);

public:
    bool save(const std::string& filename, bool verbose = true) const;
    bool load(const std::string& filename, bool verbose = true);

public:
    void clear();
    void resize(int point_count, int scale_count);

public:
    inline const Vector3& normal(int i, int j) const;
    inline       Vector3& normal(int i, int j);

    inline const Vector2& curvatures(int i, int j) const;
    inline       Vector2& curvatures(int i, int j);

    inline const Scalar& k1(int i, int j) const;
    inline       Scalar& k1(int i, int j);

    inline const Scalar& k2(int i, int j) const;
    inline       Scalar& k2(int i, int j);

    inline Scalar plane_dev(int i, int j) const;

public:
    inline int index(int i, int j) const;

public:
    int m_point_count;
    int m_scale_count;
    std::vector<Vector3> m_normals;
    std::vector<Vector2> m_curvatures;
};

} // namespace pdpc

#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.inl>
