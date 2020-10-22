#pragma once

#include <string>

namespace pdpc {

class PointCloud;

class OBJ
{
public:
    static bool load(const std::string& filename,       PointCloud& g, bool verbose = true);
    static bool load(const std::string& filename,       PointCloud& g, std::string& texture, bool verbose = true);
    static bool load_soup(const std::string& filename,  PointCloud& g, bool verbose = true);
    static bool load_soup(const std::string& filename,  PointCloud& g, std::string& texture, bool verbose = true);
    static bool save(const std::string& filename, const PointCloud& g, bool verbose = true);
    static bool save(const std::string& filename, const PointCloud& g, const std::string& texture, bool verbose = true);
};

} // namespace pdpc
