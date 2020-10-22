#pragma once

namespace pdpc {

class abstract_vector_index_iterator
{
public:
    abstract_vector_index_iterator();
    abstract_vector_index_iterator(const abstract_vector_index_iterator& other);
    abstract_vector_index_iterator(int index);

    abstract_vector_index_iterator& operator += (int n);
    abstract_vector_index_iterator& operator -= (int n);
    abstract_vector_index_iterator& operator ++ ();
    abstract_vector_index_iterator& operator -- ();

    abstract_vector_index_iterator operator + (int n) const;
    abstract_vector_index_iterator operator - (int n) const;

    bool operator == (const abstract_vector_index_iterator& other) const;
    bool operator != (const abstract_vector_index_iterator& other) const;

    int operator * () const;
    int index() const;

protected:
    int m_index;
};

} // namespace pdpc
