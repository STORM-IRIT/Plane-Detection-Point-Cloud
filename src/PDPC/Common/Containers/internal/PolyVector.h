#pragma once

#include <PDPC/Common/Containers/internal/AbstractVector.h>

#include <memory>

namespace pdpc {

class poly_vector;

// =============================================================================

template<bool is_const>
class poly_vector_view;

template<>
class poly_vector_view<true>
{
public:
    poly_vector_view(const poly_vector* poly_vector, int index = -1);

    poly_vector_view(const poly_vector_view<true>& other) = delete;
    poly_vector_view& operator =(const poly_vector_view<true>& other) = delete;

public:
    int  index() const;
    int& index();

    const poly_vector* get_poly_vector() const;

public:
    template<typename T>
    inline const T& at(int idx) const;

protected:
    const poly_vector* m_poly_vector;
    int m_index;
};

// =============================================================================

template<>
class poly_vector_view<false>
{
public:
    poly_vector_view(poly_vector* poly_vector, int index = -1);

    poly_vector_view(const poly_vector_view<false>& other);
    poly_vector_view& operator =(const poly_vector_view<false>& other);

public:
    int  index() const;
    int& index();

    const poly_vector* get_poly_vector() const;
          poly_vector* get_poly_vector();

public:
      template<typename T>
      inline const T& at(int idx) const;

      template<typename T>
      inline T& at(int idx);

protected:
    poly_vector* m_poly_vector;
    int m_index;
};

void swap(poly_vector_view<false> a, poly_vector_view<false> b);

// =============================================================================

template<bool is_const>
class poly_vector_iterator
{
public:
    using vector   = typename std::conditional<is_const, const poly_vector, poly_vector>::type;
    using iterator = abstract_vector_iterator_interface<is_const>;
    using self     = poly_vector_iterator<is_const>;
    using view     = poly_vector_view<is_const>;

    using value_type        = view;
    using reference         = view&;
    using pointer           = view*;
    using difference_type   = int;
    using iterator_category = std::random_access_iterator_tag;

public:
    inline poly_vector_iterator(vector* v, int n);

    template<bool is_other_const>
    inline bool match(const poly_vector_iterator<is_other_const>& other) const;

    inline int count() const;

public:
    inline self& operator -- ();
    inline self& operator ++ ();

    inline bool operator != (const self& other) const;
    inline bool operator == (const self& other) const;
    inline bool operator <  (const self& other) const;

    inline int operator - (const self& other) const;

    inline self operator + (int i) const;
    inline self operator - (int i) const;

    inline view operator * ();

public:
    inline const iterator& operator [](int idx) const;
    inline       iterator& operator [](int idx);

protected:
    vector* m_poly_vector;
    std::vector<iterator> m_iterators;
    int m_current;
};

// =============================================================================

template<bool is_const>
inline int distance(poly_vector_iterator<is_const> a, poly_vector_iterator<is_const> b);

// =============================================================================

class poly_vector
{
    friend class poly_vector_iterator<true>;
    friend class poly_vector_iterator<false>;

public:
    using vec_ptr        = std::unique_ptr<abstract_vector>;
    using iterator       = poly_vector_iterator<false>;
    using const_iterator = poly_vector_iterator<true>;

    // Constructor -------------------------------------------------------------
public:
    poly_vector();
    poly_vector(const poly_vector& other);
    poly_vector(poly_vector&& other);
    poly_vector(int size);
    ~poly_vector();

    bool match(const poly_vector& other) const;

    void transfer_from(const poly_vector& other);

    void assign(const_iterator first, const_iterator last);

    poly_vector& operator = (const poly_vector& other);
    poly_vector& operator = (poly_vector&& other);

    inline int count() const;
    void clean();

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Iterator ----------------------------------------------------------------
public:
    inline iterator begin();
    inline iterator end();
    inline const_iterator cbegin();
    inline const_iterator cend();
    inline const_iterator begin() const;
    inline const_iterator end() const;

    // Capacity ----------------------------------------------------------------
public:
    inline bool empty() const;
    inline int size() const;
    void reserve(int new_cap);
    void shrink_to_fit();

    // Modifiers ---------------------------------------------------------------
public:
    void clear();
    void insert(const iterator& pos, const const_iterator& first, const const_iterator& last);
    void erase(int pos);
    void erase(int first, int last);
    void push_back();
    void emplace_back();
    void pop_back();
    void resize(int size);
    void swap(int i, int j);
    void set(int target, int source);

    // Add/Remove --------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline int add();
    inline int add(IO::TypeID type);

    template<typename T, class Allocator = typename allocator<T>::type>
    inline void discard(int idx);
    inline void discard(int idx);
    inline void discard_all();

    // Accessors ---------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline const T& at(int idx, int n) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline T& at(int idx, int n);

    template<typename T, class Allocator = typename allocator<T>::type>
    inline const std::vector<T,Allocator>& at(int idx) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline std::vector<T,Allocator>& at(int idx);

    template<typename T, class Allocator = typename allocator<T>::type>
    inline const std::shared_ptr<std::vector<T,Allocator>>& ptr(int idx) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline std::shared_ptr<std::vector<T,Allocator>>& ptr(int idx);

    inline const abstract_vector* at(int idx) const;

    // Checking ----------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline bool has(int idx) const;
    bool has(int idx) const;

    bool _is_valid() const;

    // Internal ----------------------------------------------------------------
protected:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector<T,Allocator>& get(int idx) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline concrete_vector<T,Allocator>& get(int idx);

    inline const abstract_vector* operator [](int idx) const;
    inline       abstract_vector* operator [](int idx);

protected:
    std::vector<vec_ptr> m_vectors;
    int m_size;

}; // class poly_vector

} // namespace pdpc

#include <PDPC/Common/Containers/internal/PolyVector.inl>
