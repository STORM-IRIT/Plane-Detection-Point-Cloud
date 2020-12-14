#include <PDPC/Segmentation/RegionSet.h>

namespace pdpc {

// RegionSet -------------------------------------------------------------------

RegionSet::RegionSet() :
    m_regions(),
    m_properties()
{
}

RegionSet::RegionSet(int region_count) :
    m_regions(region_count),
    m_properties(region_count)
{
}

// IO --------------------------------------------------------------------------

std::ostream& RegionSet::write(std::ostream& os) const
{
    int region_count = size();
    IO::write_value(os, region_count);
    for(int i=0; i<region_count; ++i)
    {
        int count = size(i);
        IO::write_value(os, count);
        //TODO write m_regions[i].data() directly
        for(int j=0; j<count; ++j)
        {
            IO::write_value(os, index(i, j));
        }
    }
    m_properties.write(os);
    return os;
}

std::istream& RegionSet::read(std::istream& is)
{
    int region_count = -1;
    IO::read_value(is, region_count);
    resize(region_count);
    for(int i=0; i<region_count; ++i)
    {
        int count = -1;
        IO::read_value(is, count);
        resize(i, count);
        //TODO read m_regions[i].data() directly
        for(int j=0; j<count; ++j)
        {
            IO::read_value(is, index(i, j));
        }
    }
    m_properties.read(is);
    return is;
}

// Modifiers -------------------------------------------------------------------

void RegionSet::clear()
{
    regions().clear();
    properties().clear();
}

void RegionSet::clear(int i)
{
    region(i).clear();
}

void RegionSet::resize(int region_count)
{
    regions().resize(region_count);
    properties().resize(region_count);
}

void RegionSet::resize(int i, int count)
{
    region(i).resize(count);
}

} // namespace pdpc
