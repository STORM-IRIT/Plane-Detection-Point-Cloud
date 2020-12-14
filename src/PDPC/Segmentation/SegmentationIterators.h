#pragma once

namespace pdpc {

class Segmentation;
class RegionIterator;

class RegionView
{
    friend class RegionIterator;
public:
    RegionView(const Segmentation& segmentation, int label);

    int label() const;
    int size() const;

protected:
    const Segmentation& m_segmentation;
    int m_label;
};

// -----------------------------------------------------------------------------

class RegionIterator
{
public:
    RegionIterator(const Segmentation& segmentation, int label);

    RegionIterator operator ++();
    bool operator !=(const RegionIterator& other) const;
    const RegionView& operator *() const;

protected:
    RegionView m_region_view;
};

// -----------------------------------------------------------------------------

class IndexIterator
{
public:
    IndexIterator(const Segmentation& segmentation, int label, bool is_begin);

    int label() const;
    int index() const;

    IndexIterator operator ++();
    bool operator !=(const IndexIterator& other) const;
    int operator *() const;

protected:
    void advance();

protected:
    const Segmentation& m_segmentation;
    int m_label;
    int m_index;
};

// -----------------------------------------------------------------------------

class IndexIteratorView
{
public:
    IndexIteratorView(const Segmentation& segmentation, int label);

    IndexIterator begin();
    IndexIterator end();

protected:
    const Segmentation& m_segmentation;
    int m_label;
};

} // namespace pdpc
