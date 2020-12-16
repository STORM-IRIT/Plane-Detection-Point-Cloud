#include <PDPC/Segmentation/Segmentation.h>

namespace pdpc {

// Capacity --------------------------------------------------------------------

int Segmentation::size() const
{
    return m_labels.size();
}

int Segmentation::region_size(int l) const
{
    return count(l);
}

int Segmentation::region_count() const
{
    return m_counts.size() - 1;
}

int Segmentation::label_count() const
{
    return m_counts.size() - 1;
}

int Segmentation::size(int l) const
{
    return count(l);
}

// Accessors -------------------------------------------------------------------

int Segmentation::label(int idx) const
{
    PDPC_DEBUG_ASSERT(0 <= idx && idx < size());
    return m_labels[idx];
}

int Segmentation::operator [](int idx) const
{
    PDPC_DEBUG_ASSERT(0 <= idx && idx < size());
    return m_labels[idx];
}

bool Segmentation::contains(int l) const
{
    return counts_size(l) <= region_count();
}

bool Segmentation::is_valid(int idx) const
{
    return label(idx) != invalid();
}

bool Segmentation::is_invalid(int idx) const
{
    return label(idx) == invalid();
}

int Segmentation::label_max() const
{
    int lmax  = label_sup();
    int limit = label_inf();
    while(lmax>=limit && count(lmax)==0)
    {
        --lmax;
    }
    return lmax==limit-1 ? invalid()-2 : lmax;
}

int Segmentation::label_min() const
{
    int lmin  = label_inf() + 1;
    int limit = label_sup();
    while(lmin<=limit && count(lmin)==0)
    {
        ++lmin;
    }
    return lmin==limit+1 ? invalid()-1 : lmin;
}

int Segmentation::label_sup() const
{
    return m_counts.size()-2;
}

int Segmentation::label_inf() const
{
    return invalid();
}

int Segmentation::invalid_count() const
{
    return count(invalid());
}

} // namespace pdpc
