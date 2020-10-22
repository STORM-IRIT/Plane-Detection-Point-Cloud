#include <PDPC/Segmentation/MSSegmentation.h>

namespace pdpc {

// Modifiers -------------------------------------------------------------------

void MSSegmentation::make_continuous()
{
    for_each([](auto& seg){seg.make_continuous();});
}

void MSSegmentation::make_valid(int l)
{
    for_each([l](auto& seg){seg.make_valid(l);});
}

bool MSSegmentation::is_valid() const
{
    for(const Segmentation& seg : m_data)
    {
        if(!seg.is_valid())
            return false;
    }
    return true;
}

} // namespace pdpc
