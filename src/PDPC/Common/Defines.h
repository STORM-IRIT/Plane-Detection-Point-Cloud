#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>

namespace pdpc
{

using Scalar = float;
using Vector2 = Eigen::Matrix<Scalar,2,1>;
using Vector3 = Eigen::Matrix<Scalar,3,1>;
using Vector4 = Eigen::Matrix<Scalar,4,1>;
using Vector3i = Eigen::Matrix<int,3,1>;
using Vector2Array = std::vector<Vector2>;
using Vector3Array = std::vector<Vector3>;
using Vector4Array = std::vector<Vector4>;
using Vector3iArray = std::vector<Vector3i>;
using Matrix3 = Eigen::Matrix<Scalar,3,3>;
using Aabb  = Eigen::AlignedBox<Scalar,3>;
using Aabb3 = Eigen::AlignedBox<Scalar,3>;
using Aabb2 = Eigen::AlignedBox<Scalar,2>;

} // namespace pdpc
