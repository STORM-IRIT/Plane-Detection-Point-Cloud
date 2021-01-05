#pragma once

#include <PDPC/ScaleSpace/ScaleSpaceStack.h>
#include <PDPC/Segmentation/Segmentation.h>

namespace pdpc {

class MSSegmentation : public ScaleSpaceStack<Segmentation>
{
public:
    using Base = ScaleSpaceStack<Segmentation>;
    using Base::Base;

public:
    bool is_valid() const;

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;

    // Modifiers ---------------------------------------------------------------
public:
    void make_continuous();
    void make_valid(int l = 0);

}; // class MSSegmentation

} // namespace pdpc

