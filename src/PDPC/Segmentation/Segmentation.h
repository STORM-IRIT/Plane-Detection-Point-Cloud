#pragma once

#include <PDPC/Segmentation/SegmentationIterators.h>
#include <PDPC/Common/Assert.h>

#include <vector>
#include <iostream>

namespace pdpc {

//!
//! \brief The Segmentation class contains a set of indexed labels.
//!
//! Elements can be set as 'unlabeled' or 'invalid' with the invalid label -1.
//! The Segmentation class keeps track of the size of each regions.
//!
//! label_inf is always equal to the invalid label -1.
//! label_sup is the maximal existing label for which a region size is defined.
//!
//! label_min is the first valid label of a non empty region (excluding -1).
//! label_max is the last  valid label of a non empty region.
//!
//! A segmentation can be :
//! - complete   if it is valid and full
//! - continuous if no label between label_min   and label_max refers to an empty region
//! - full       if no label between label_inf+1 and label_sup refers to an empty region
//! - valid      if every elements are valid
//!
//! complete => full + valid => continuous
//! Full     => continuous
//!
class Segmentation
{
    // Types -------------------------------------------------------------------
public:
    inline static constexpr int invalid() {return -1;}
    inline static constexpr int INVALID() {return -1;} //TODO remove this
    inline static constexpr int Invalid() {return -1;} //TODO remove this

    // Segmentation ------------------------------------------------------------
public:
    Segmentation();
    Segmentation(int size, int l = invalid());
    Segmentation(const std::vector<int>& labels);
    Segmentation(const Segmentation& other);

    bool is_consistent() const;

    void fill(std::vector<std::vector<int>>& seg) const;

    // Equal -------------------------------------------------------------------
public:
    bool operator == (const Segmentation& other) const;
    bool operator == (const std::vector<int>& labels) const;

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);
    bool save(const std::string& filename) const;

    // Iterators ---------------------------------------------------------------
public:
    RegionIterator begin() const; // start at the invalid region
    RegionIterator end() const;

    IndexIterator begin(int l) const;
    IndexIterator end(int l) const;

    IndexIteratorView indices(int l) const;
    IndexIteratorView indices(const RegionView& region) const;

    // Capacity ----------------------------------------------------------------
public:
    inline int size() const;
    inline int region_size(int l) const;
    inline int region_count() const; // invalid region not included
    int non_empty_region_count() const; // invalid region not included

    inline int size(int l) const;   //TODO remove this
    inline int label_count() const; //TODO remove this

    void resize(int size, int l = invalid());
    void resize_region(int region_count);

    // Warning!
    // resize_region(N)
    // M = region_count()
    // N != M

    void reserve(int capacity);
    void reserve_region(int region_capacity);

    // Modifiers ---------------------------------------------------------------
public:
    void clear();
    void reset(int l = invalid());

    void push_back(int l);
    void set_label(int idx, int l);

    void invalidate_region(int label);
    void invalidate_regions(const std::vector<bool>& to_invalidate);
    void invalidate(const std::vector<bool>& to_unlabel);
    void invalidate_small_region(int min_count);

    void unlabelSmallSize(int min_count); //TODO remove this

    int new_label();

    void merge(int l1, int l2);

    void swap_label(int l1, int l2);

    // Accessors ---------------------------------------------------------------
public:
    inline int label(int idx) const;
    inline int operator [](int idx) const;

    inline bool contains(int l) const;
    inline bool is_valid(int idx) const;
    inline bool is_invalid(int idx) const;

    inline int label_min() const; // the first valid label of a non empty region (excluding -1)
    inline int label_max() const; // the last  valid label of a non empty region

    inline int label_inf() const; // the first existing label = invalid = -1
    inline int label_sup() const; // the last  existing label

    inline int invalid_count() const;

    // States ------------------------------------------------------------------
public:
    bool is_complete() const;
    bool is_valid() const;
    bool is_continuous() const;
    bool is_full() const;

    void make_valid(int l = 0);
    void make_continuous();
    void make_full();

    // Coloring ----------------------------------------------------------------
public:
    template<class ColorArrayT, class ColorT, class PaletteT>
    void set_colors(ColorArrayT& colors,
                    const ColorT& invalid,
                    const PaletteT& palette) const;

    // Internal ----------------------------------------------------------------
protected:
    int  count(int l) const;
    int& count(int l);

    void compute_counts();
    static int counts_size(int l);

    // Data --------------------------------------------------------------------
public:
    std::vector<int> m_labels;
    std::vector<int> m_counts;
};

// Coloring --------------------------------------------------------------------

template<class ColorArrayT, class ColorT, class ColormapT>
void Segmentation::set_colors(ColorArrayT& colors,
                              const ColorT& invalid,
                              const ColormapT& colormap) const
{
    colors.resize(this->size());
    for(int idx=0; idx<this->size(); ++idx)
    {
        colors[idx] = this->is_invalid(idx) ? invalid : colormap[this->label(idx)];
    }
}

} // namespace pdpc


#include <PDPC/Segmentation/Segmentation.inl>
