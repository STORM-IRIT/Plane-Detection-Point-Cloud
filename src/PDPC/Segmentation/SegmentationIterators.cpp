#include <PDPC/Segmentation/SegmentationIterators.h>
#include <PDPC/Segmentation/Segmentation.h>

namespace pdpc {

RegionView::RegionView(const Segmentation& segmentation, int label) :
    m_segmentation(segmentation),
    m_label(label)
{
}

int RegionView::label() const
{
    return m_label;
}

int RegionView::size() const
{
    return m_segmentation.region_size(m_label);
}

// -----------------------------------------------------------------------------

RegionIterator::RegionIterator(const Segmentation& segmentation, int label) :
    m_region_view(segmentation, label)
{
}

RegionIterator RegionIterator::operator ++()
{
    ++m_region_view.m_label;
    return *this;
}

bool RegionIterator::operator !=(const RegionIterator& other) const
{
    return m_region_view.m_label != other.m_region_view.m_label;
}

const RegionView& RegionIterator::operator *() const
{
    return m_region_view;
}

// -----------------------------------------------------------------------------

IndexIterator::IndexIterator(const Segmentation& segmentation, int label, bool is_begin) :
    m_segmentation(segmentation),
    m_label(label),
    m_index(0)
{
    if(is_begin)
    {
        m_index = -1;
        this->advance();
    }
    else
    {
        m_index = m_segmentation.size();
    }
}

int IndexIterator::label() const
{
    return m_label;
}

int IndexIterator::index() const
{
    return m_index;
}

IndexIterator IndexIterator::operator ++()
{
    this->advance();
    return *this;
}

bool IndexIterator::operator != (const IndexIterator& other) const
{
    return m_index != other.m_index;
}

int IndexIterator::operator *() const
{
    return m_index;
}

void IndexIterator::advance()
{
    do
    {
        ++m_index;
    }
    while(m_index < m_segmentation.size() && m_segmentation[m_index] != m_label);
}

// -----------------------------------------------------------------------------

IndexIteratorView::IndexIteratorView(const Segmentation& segmentation, int label) :
    m_segmentation(segmentation),
    m_label(label)
{
}

IndexIterator IndexIteratorView::begin()
{
    return m_segmentation.begin(m_label);
}

IndexIterator IndexIteratorView::end()
{
    return m_segmentation.end(m_label);
}

} // namespace pdpc
