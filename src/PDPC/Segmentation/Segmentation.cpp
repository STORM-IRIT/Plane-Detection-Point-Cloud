#include <PDPC/Segmentation/Segmentation.h>
#include <PDPC/Common/Assert.h>

#include <algorithm>
#include <numeric>
#include <fstream>

namespace pdpc {

// Segmentation ----------------------------------------------------------------

Segmentation::Segmentation() :
    m_labels(),
    m_counts()
{
}

Segmentation::Segmentation(int size, int l) :
    m_labels(size, l),
    m_counts(counts_size(l), 0)
{
    this->count(l) = size;
}

Segmentation::Segmentation(const std::vector<int>& labels) :
    m_labels(labels),
    m_counts()
{
    this->compute_counts();
}

Segmentation::Segmentation(const Segmentation& other) :
    m_labels(other.m_labels),
    m_counts(other.m_counts)
{
}

bool Segmentation::is_consistent() const
{
    std::vector<int> counts(m_counts.size(), 0);
    for(uint idx=0; idx<m_labels.size(); ++idx)
    {
        const int l = this->label(idx);
        const int c = l+1; // +1 for the invalid region
        PDPC_DEBUG_ASSERT(0 <= c && c < int(counts.size()));
        ++counts[c];
    }

    const int this_size = std::accumulate(m_counts.begin(), m_counts.end(), 0);
    if(this_size != int(m_labels.size()))
    {
        PDPC_DEBUG_ERROR;
        return false;
    }

    const int actual_size = std::accumulate(counts.begin(), counts.end(), 0);
    if(actual_size != int(m_labels.size()))
    {
        PDPC_DEBUG_ERROR;
        return false;
    }

    if(counts != m_counts)
    {
        PDPC_DEBUG_ERROR;
        return false;
    }
    return true;
}

void Segmentation::fill(std::vector<std::vector<int>>& seg) const
{
    seg.clear();
    seg.resize(region_count());

    for(int idx=0; idx<size(); ++idx)
    {
        const int l = label(idx);
        if(l != invalid())
        {
            PDPC_DEBUG_ASSERT(0 <= l);
            PDPC_DEBUG_ASSERT(l < int(seg.size()));
            seg[l].push_back(idx);
        }
    }
}

// Equal -----------------------------------------------------------------------

bool Segmentation::operator == (const Segmentation& other) const
{
    return m_labels == other.m_labels;
}

bool Segmentation::operator == (const std::vector<int>& labels) const
{
    return m_labels == labels;
}

// IO --------------------------------------------------------------------------

std::ostream& Segmentation::write(std::ostream& os) const
{
    PDPC_DEBUG_ASSERT(this->is_consistent());
    const int size = this->size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(int));
    os.write(reinterpret_cast<const char*>(m_labels.data()), size*sizeof(int));
    return os;
}

std::istream& Segmentation::read(std::istream& is)
{
    int size = -1;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));
    PDPC_DEBUG_ASSERT(0 <= size);

    m_labels.resize(size);
    is.read(reinterpret_cast<char*>(m_labels.data()), size*sizeof(int));

    this->compute_counts();
    PDPC_DEBUG_ASSERT(this->is_consistent());
    return is;
}

bool Segmentation::save(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        PDPC_DEBUG_ERROR;
        return false;
    }

    for(int l : m_labels) ofs << l << "\n";

    return true;
}

// Iterators -------------------------------------------------------------------

RegionIterator Segmentation::begin() const
{
    return RegionIterator(*this, this->label_inf());
}

RegionIterator Segmentation::end() const
{
    return RegionIterator(*this, this->label_sup() + 1); // +1 to be 'just after' the last region
}

IndexIterator Segmentation::begin(int l) const
{
    return IndexIterator(*this, l, true);
}

IndexIterator Segmentation::end(int l) const
{
    return IndexIterator(*this, l, false);
}

IndexIteratorView Segmentation::indices(int l) const
{
    return IndexIteratorView(*this, l);
}

IndexIteratorView Segmentation::indices(const RegionView& region) const
{
    return indices(region.label());
}

// Capacity --------------------------------------------------------------------

int Segmentation::non_empty_region_count() const
{
    return std::count_if(m_counts.begin()+1, m_counts.end(), [](int size){return size > 0;});
}

void Segmentation::resize(int size, int l)
{
    const int old_size = this->size();
    if(size < old_size)
    {
        for(int idx=size; idx<old_size; ++idx)
        {
            --count(label(idx));
        }
        m_labels.resize(size);
    }
    else if(size > old_size)
    {
        if(l > label_max())
        {
            m_counts.resize(counts_size(l), 0);
        }
        const int diff  = size - old_size;
        count(l) += diff;
        m_labels.resize(size, l);
    }
}

void Segmentation::resize_region(int region_count)
{
    PDPC_DEBUG_ASSERT(region_count > this->region_count()); // we can only add empty region
    m_counts.resize(region_count, 0);
}

void Segmentation::reserve(int capacity)
{
    m_labels.reserve(capacity);
}

void Segmentation::reserve_region(int region_capacity)
{
    m_counts.reserve(region_capacity);
}

// Modifiers -------------------------------------------------------------------

void Segmentation::clear()
{
    m_labels.clear();
    m_labels.clear();
}

void Segmentation::reset(int l)
{
    std::fill(m_labels.begin(), m_labels.end(), l);
    m_counts.resize(counts_size(l), 0);
    std::fill(m_counts.begin(), m_counts.end(), 0);
    count(l) = size();
}

void Segmentation::push_back(int l)
{
    if(counts_size(l) > int(m_counts.size()))
        m_counts.resize(counts_size(l));
    ++count(l);
    m_labels.emplace_back(l);
}

void Segmentation::set_label(int idx, int l)
{
    const int old_label = label(idx);

    --count(old_label);
    ++count(l);

    m_labels[idx] = l;
}

void Segmentation::invalidate_region(int l)
{
    for(int idx=0; idx<size(); ++idx)
    {
        if(label(idx) == l)
        {
            m_labels[idx] = invalid();
        }
    }

    count(invalid()) += count(l);
    count(l) = 0;
}

//!
//! \brief invalidate several regions
//! \param to_invalidate says if a region of label l is to be invalidated
//!
//! The size of to_invalidate must be equal to region_count() : it does not
//! include the invalid label (-1).
//!
void Segmentation::invalidate_regions(const std::vector<bool>& to_invalidate)
{
    PDPC_DEBUG_ASSERT(int(to_invalidate.size()) == region_count());

    int invalidated_count = 0;
    for(int idx=0; idx<size(); ++idx)
    {
        const int l = label(idx);
        if(l != invalid() && to_invalidate[l])
        {
            m_labels[idx] = invalid();
            ++invalidated_count;
        }
    }

    for(int l=0; l<region_count(); ++l)
    {
        if(to_invalidate[l])
        {
            count(l) = 0;
        }
    }
    count(invalid()) += invalidated_count;
}

//TODO make this clear:
// same as invalidate_regions() but to_unlabel goes from label_inf to label_sup
void Segmentation::invalidate(const std::vector<bool>& to_unlabel)
{
    int count = 0;
    int linf  = label_inf();

    for(int idx=0; idx<size(); ++idx)
    {
        int l = label(idx);
        if(to_unlabel[l-linf])
        {
            m_labels[idx] = invalid();
            ++count;
        }
    }
    for(int l=label_inf(); l<=label_sup(); ++l)
    {
        if(to_unlabel[l-linf])
        {
            this->count(l) = 0;
        }
    }
    this->count(invalid()) += count;
}

void Segmentation::invalidate_small_region(int min_count)
{
    std::vector<bool> b(m_counts.size());
    int linf = label_inf();

    for(int l=label_inf(); l<=label_sup(); ++l)
    {
        b[l-linf] = count(l) < min_count;
    }
    invalidate(b);
}

void Segmentation::unlabelSmallSize(int min_count)
{
    invalidate_small_region(min_count);
}

int Segmentation::new_label()
{
    int l = label_sup() + 1;
    m_counts.emplace_back(0);
    return l;
}

void Segmentation::merge(int l1, int l2)
{
    // each l2 becomes l1
    for(int idx=0; idx<size(); ++idx)
    {
        if(m_labels[idx] == l2)
        {
            --count(l2);
            ++count(l1);
            m_labels[idx] = l1;
        }
    }
}

void Segmentation::swap_label(int l1, int l2)
{
    // each l2 becomes l1
    // each l1 becomes l2
    for(int idx=0; idx<size(); ++idx)
    {
        if(m_labels[idx] == l2)
        {
            --count(l2);
            ++count(l1);
            m_labels[idx] = l1;
        }
        else if(m_labels[idx] == l1)
        {
            --count(l1);
            ++count(l2);
            m_labels[idx] = l2;
        }
    }
}

// States ----------------------------------------------------------------------

bool Segmentation::is_complete() const
{
    return is_valid() && is_full();
}

bool Segmentation::is_valid() const
{
    return invalid_count() == 0;
}

bool Segmentation::is_continuous() const
{
    if(m_counts.empty())
        return true;

    auto begin = m_counts.begin() + 1 + label_min();
    auto end   = m_counts.begin() + 1 + label_max();
    auto it    = std::find(begin, end, 0);
    return it == end;
}

bool Segmentation::is_full() const
{
    if(m_counts.empty())
        return true;

    auto begin = m_counts.begin() + 1 + label_inf() + 1;
    auto end   = m_counts.begin() + 1 + label_sup();
    auto it    = std::find(begin, end, 0);
    return it == end;
}

void Segmentation::make_valid(int l)
{
    int s = invalid_count();
    for(int idx=0; idx<size(); ++idx)
    {
        if(is_invalid(idx))
        {
            m_labels[idx] = l;
        }
        else if(m_labels[idx] >= l)
        {
            ++m_labels[idx];
        }
    }
    m_counts.insert(m_counts.begin()+l, s);
    count(Invalid()) = 0;
}

void Segmentation::make_continuous()
{
    int lmin = label_min();
    int lmax = label_max();

    std::vector<int> decr(m_counts.size(), 0);
    for(int l=lmin+1; l<=lmax; ++l)
    {
        decr[l] = decr[l-1] + (count(l-1)==0 ? 1 : 0);
    }

    for(int idx=0; idx<size(); ++idx)
    {
        if(is_valid(idx))
        {
            int old_label = m_labels[idx];
            int new_label = m_labels[idx] - decr[m_labels[idx]];
            --count(old_label);
            ++count(new_label);
            m_labels[idx] = new_label;
        }
    }
}

void Segmentation::make_full()
{
    int lmax = this->label_max();

    std::vector<int> decr(m_counts.size(), 0);
    for(int l=1; l<=lmax; ++l)
    {
        decr[l] = decr[l-1] + (count(l-1)==0 ? 1 : 0);
    }

    for(int idx=0; idx<size(); ++idx)
    {
        if(is_valid(idx))
        {
            int old_label = m_labels[idx];
            int new_label = m_labels[idx] - decr[m_labels[idx]];
            --count(old_label);
            ++count(new_label);
            m_labels[idx] = new_label;
        }
    }
    m_counts.erase(m_counts.begin()+label_max()+2, m_counts.end());
    // +2 = avoid invalid region + advance just after label_max
}

// Internal --------------------------------------------------------------------

int Segmentation::count(int l) const
{
    PDPC_DEBUG_ASSERT(0 <= l+1 && l+1 < int(m_counts.size()));
    return m_counts[l+1];
}

int& Segmentation::count(int l)
{
    PDPC_DEBUG_ASSERT(0 <= l+1 && l+1 < int(m_counts.size()));
    return m_counts[l+1];
}

void Segmentation::compute_counts()
{
    int lmax = *std::max_element(m_labels.begin(), m_labels.end());
    m_counts.resize(counts_size(lmax));
    std::fill(m_counts.begin(), m_counts.end(), 0);

    for(int l : m_labels)
    {
        ++count(l);
    }
}

int Segmentation::counts_size(int l)
{
    return l+2;
}

} // namespace pdpc
