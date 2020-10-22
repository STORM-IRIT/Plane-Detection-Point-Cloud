#pragma once

#include <string>

namespace pdpc {

class PointCloud;

class Loader
{
public:
    static bool Load(const std::string& filename, PointCloud& g, bool verbose = true);
    static bool Save(const std::string& filename, const PointCloud& g, bool verbose = true);

};

} // namespace pdpc
