#pragma once

#include <string>

namespace pdpc {

class PointCloud;

class PLY
{
public:
    static bool load(const std::string& filename,       PointCloud& g, bool verbose = true);
    static bool save(const std::string& filename, const PointCloud& g, bool verbose = true, bool binary = true);
};

} // namespace pdpc
