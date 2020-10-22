#pragma once

#include <vector>
#include <iostream>

namespace pdpc {

class Component
{
    // Component ---------------------------------------------------------------
public:
    Component();
    Component(int index);
    Component(int birth_level, int index);

    bool is_valid() const;

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Accessors ---------------------------------------------------------------
public:
    int size() const;
    bool empty() const;

    int birth_level() const;
    int death_level() const;
    int persistence() const;

    int index(int level) const;
    int operator[] (int level) const;

    int level_at(int i) const;
    int index_at(int i) const;

    // Modifiers ---------------------------------------------------------------
public:
    void clear();

    void set_index(int level, int index);
    void push_back(int index);

    void initialize(int birth_level, int index);

    void reserve(int cap);

    // Internal ----------------------------------------------------------------
protected:
    int  index_at_level(int level) const;
    int& index_at_level(int level);

    // Data --------------------------------------------------------------------
protected:
    int              m_birth_level;
    std::vector<int> m_indices;

}; // class Component

} // namespace pdpc

