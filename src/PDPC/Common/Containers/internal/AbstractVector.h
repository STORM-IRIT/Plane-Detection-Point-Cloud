#pragma once

#include <PDPC/Common/Containers/internal/abstract_vector_index_iterator.h>
#include <PDPC/Common/IO.h>

#include <memory>
#include <vector>

namespace pdpc {

template<typename T>
struct allocator {using type = std::allocator<T>;};

// forward declarations
template<typename T, class Allocator = typename allocator<T>::type>
class concrete_vector;
class abstract_vector;
template<bool is_const, typename T, class Allocator = typename allocator<T>::type>
class concrete_vector_iterator;
template<bool is_const>
class abstract_vector_iterator;
template<bool is_const>
class abstract_vector_iterator_interface;
template<bool is_const, typename T, class Allocator = typename allocator<T>::type>
class concrete_vector_view;
template<bool is_const>
class abstract_vector_view;

// =============================================================================

template<bool is_const>
class abstract_vector_view;

template<>
class abstract_vector_view<true>
{
public:
    abstract_vector_view(){}
    virtual ~abstract_vector_view(){}

    abstract_vector_view(const abstract_vector_view<true>& other) = delete;
    abstract_vector_view& operator = (const abstract_vector_view<true>& other) = delete;

    // Accessors ---------------------------------------------------------------
public:
    template<typename T>
    inline const T& value() const;

    // Inheritance -------------------------------------------------------------
public:
    template<class T, class Allocator = typename allocator<T>::type>
    inline concrete_vector_view<true,T,Allocator>& cast();

    template<class T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector_view<true,T,Allocator>& cast() const;

    template<class T, class Allocator = typename allocator<T>::type>
    inline bool is() const;
};

template<>
class abstract_vector_view<false>
{
public:
    abstract_vector_view(){}
    virtual ~abstract_vector_view(){}

    abstract_vector_view(const abstract_vector_view<false>& other) = delete;
    abstract_vector_view& operator = (const abstract_vector_view<false>& other) = delete;

    // Accessors ---------------------------------------------------------------
public:
    template<typename T>
    inline const T& value() const;
    template<typename T>
    inline       T& value();

    // Inheritance -------------------------------------------------------------
public:
    template<class T, class Allocator = typename allocator<T>::type>
    inline concrete_vector_view<false,T,Allocator>& cast();

    template<class T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector_view<false,T,Allocator>& cast() const;

    template<class T, class Allocator = typename allocator<T>::type>
    inline bool is() const;
};

// =============================================================================

template<bool is_const, typename T, class Allocator>
class concrete_vector_view;

template<typename T, class Allocator>
class concrete_vector_view<true,T,Allocator> : public abstract_vector_view<true>
{
    friend concrete_vector_iterator<true,T,Allocator>;

public:
    using const_iterator = typename std::vector<T,Allocator>::const_iterator;

public:
    inline concrete_vector_view(const_iterator it);

    concrete_vector_view(const concrete_vector_view<true,T,Allocator>& other) = delete;
    concrete_vector_view& operator = (const concrete_vector_view<true,T,Allocator>& other) = delete;

    // Accessors ---------------------------------------------------------------
public:
    inline const T& value() const;

protected:
    const_iterator m_iterator;
};

// =============================================================================

template<typename T, class Allocator>
class concrete_vector_view<false,T,Allocator> : public abstract_vector_view<false>
{
    friend concrete_vector_iterator<false,T,Allocator>;

public:
    using iterator = typename std::vector<T,Allocator>::iterator;

public:
    inline concrete_vector_view(iterator it);

    concrete_vector_view(const concrete_vector_view<false,T,Allocator>& other) = delete;
    concrete_vector_view& operator = (const concrete_vector_view<false,T,Allocator>& other) = delete;

    // Accessors ---------------------------------------------------------------
public:
    inline const T& value() const;
    inline       T& value();

protected:
    iterator m_iterator;
};

// =============================================================================

template<bool is_const>
class abstract_vector_iterator
{
public:
    using self          = abstract_vector_iterator<is_const>;
    using abstract_view = abstract_vector_view<is_const>;

public:
    virtual ~abstract_vector_iterator(){}
    virtual abstract_vector_iterator* clone() const = 0;

    virtual bool match(const abstract_vector_iterator<true>&  other) const = 0;
    virtual bool match(const abstract_vector_iterator<false>& other) const = 0;

    // Iterator ----------------------------------------------------------------
public:
    virtual void increment(int n) = 0;
    virtual bool equal_to(const self& other) const = 0;
    virtual bool less_than(const self& other) const = 0;
    virtual int substract(const self& other) const = 0;
    virtual abstract_view* get() = 0;

    // Inheritance -------------------------------------------------------------
public:
    template<class T, class Allocator = typename allocator<T>::type>
    inline concrete_vector_iterator<is_const,T,Allocator>& cast();

    template<class T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector_iterator<is_const,T,Allocator>& cast() const;

    template<class T, class Allocator = typename allocator<T>::type>
    inline bool is() const;
};

// =============================================================================

template<bool is_const, typename T, class Allocator>
class concrete_vector_iterator : public abstract_vector_iterator<is_const>
{
public:
    using vector_iterator       = typename std::vector<T,Allocator>::iterator;
    using const_vector_iterator = typename std::vector<T,Allocator>::const_iterator;
    using iterator              = typename std::conditional<is_const, const_vector_iterator, vector_iterator>::type;
    using concrete_view         = concrete_vector_view<is_const,T,Allocator>;
    using base                  = abstract_vector_iterator<is_const>;
    using abstract_view         = typename base::abstract_view;

public:
    inline concrete_vector_iterator(iterator it);
    inline concrete_vector_iterator(const concrete_vector_iterator<is_const,T,Allocator>& other);

    virtual base* clone() const override;

    virtual bool match(const abstract_vector_iterator<true>&  other) const override;
    virtual bool match(const abstract_vector_iterator<false>& other) const override;

public:
    virtual void increment(int n) override;
    virtual bool equal_to(const base& other) const override;
    virtual bool less_than(const base& other) const override;
    virtual int substract(const base& other) const override;
    virtual abstract_view* get() override;

public:
    inline iterator& get_iterator();

protected:
    concrete_view m_view;
};

// =============================================================================

template<bool is_const>
class abstract_vector_iterator_interface
{
public:
    using self            = abstract_vector_iterator_interface<is_const>;
    using abstract_view   = abstract_vector_view<is_const>;
    template<typename T, class Allocator>
    using vector_iterator = typename std::conditional<is_const, typename std::vector<T,Allocator>::const_iterator, typename std::vector<T,Allocator>::iterator>::type;

public:
    inline abstract_vector_iterator_interface(abstract_vector_iterator<is_const>* ptr = nullptr);
    inline abstract_vector_iterator_interface(const self& other);

    template<bool is_other_const>
    inline bool match(const abstract_vector_iterator_interface<is_other_const>& other) const;

    inline abstract_vector_iterator_interface& operator = (const self& other);

public:
    inline abstract_vector_iterator_interface& operator += (int n);
    inline abstract_vector_iterator_interface& operator -= (int n);
    inline abstract_vector_iterator_interface& operator ++ ();
    inline abstract_vector_iterator_interface& operator -- ();

    inline abstract_vector_iterator_interface operator + (int n) const;
    inline abstract_vector_iterator_interface operator - (int n) const;

    inline int operator - (const self& other) const;

    inline bool operator == (const self& other) const;
    inline bool operator != (const self& other) const;
    inline bool operator < (const self& other) const;

    //TODO make it ref?
    inline abstract_view* operator * () const;
    inline abstract_view* operator * ();

public:
    template<typename T, class Allocator>
    inline vector_iterator<T, Allocator>& iterator();
    inline const abstract_vector_iterator<is_const>* interface() const;

protected:
    std::unique_ptr<abstract_vector_iterator<is_const>> m_iterator;
};

// =============================================================================

class abstract_vector
{
    // Types -------------------------------------------------------------------
public:
    using index_iterator = abstract_vector_index_iterator;
    using iterator       = abstract_vector_iterator_interface<false>;
    using const_iterator = abstract_vector_iterator_interface<true>;

    // Clone/Create ------------------------------------------------------------
public:
    virtual abstract_vector* clone() const = 0;
    virtual abstract_vector* create() const = 0;
    virtual abstract_vector* create(int size) const = 0;

    // Constructor -------------------------------------------------------------
public:
    static inline abstract_vector* create(IO::TypeID type);
    static inline abstract_vector* create(IO::TypeID type, int size);

    virtual void assign(const_iterator first, const_iterator last) = 0;

    // Destructor --------------------------------------------------------------
public:
    virtual ~abstract_vector(){}

    // IO ----------------------------------------------------------------------
public:
    virtual IO::TypeID type_id() const = 0;
    virtual std::ostream& write(std::ostream& os) const = 0;
    virtual std::istream& read(std::istream& is) = 0;
    virtual std::string to_string(int pos) const = 0;
    virtual void from_string(int pos, const std::string& str) = 0;

    // Iterators ---------------------------------------------------------------
public:
    inline index_iterator index_begin() const;
    inline index_iterator index_end() const;

    virtual iterator begin() = 0;
    virtual iterator end() = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;

    // Capacity ----------------------------------------------------------------
public:
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual bool max_size() const = 0;
    virtual void reserve(int new_cap) = 0;
    virtual int capacity() const = 0;
    virtual void shrink_to_fit() = 0;

    // Modifiers ---------------------------------------------------------------
public:
    virtual void clear() = 0;
    virtual void insert(iterator pos, const_iterator first, const_iterator last) = 0;
    virtual void erase(index_iterator pos) = 0;
    virtual void erase(index_iterator first, index_iterator last) = 0;
    virtual void push_back() = 0;
    virtual void emplace_back() = 0;
    virtual void pop_back() = 0;
    virtual void resize(int count) = 0;
    virtual void swap(abstract_vector& other) = 0;
    virtual void swap(index_iterator pos1, index_iterator pos2) = 0;
    virtual void set(index_iterator target, index_iterator source) = 0;

    // Inheritance -------------------------------------------------------------
public:
    template<class T, class Allocator = typename allocator<T>::type>
    inline concrete_vector<T,Allocator>& cast();

    template<class T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector<T,Allocator>& cast() const;

    template<class T, class Allocator = typename allocator<T>::type>
    inline bool is() const;

    virtual bool match(const abstract_vector& other) const = 0;
};

// =============================================================================

template<typename T, class Allocator>
class concrete_vector : public abstract_vector
{
    // Types -------------------------------------------------------------------
public:
    using value_type        = T;
    using allocator_type    = Allocator;
    using size_type         = int;
    using difference_type   = int;
    using reference         = value_type&;
    using const_reference   = const value_type&;

    // Constructors ------------------------------------------------------------
public:
    inline concrete_vector();
    inline concrete_vector(int size);
    inline concrete_vector(const concrete_vector<T,Allocator>& other);

    virtual void assign(const_iterator first, const_iterator last) override;

    // Clone/Create ------------------------------------------------------------
public:
    virtual abstract_vector* clone() const override;
    virtual abstract_vector* create() const override;
    virtual abstract_vector* create(int size) const override;

    // IO ----------------------------------------------------------------------
public:
    virtual IO::TypeID type_id() const override;
    virtual std::ostream& write(std::ostream& os) const override;
    virtual std::istream& read(std::istream& is) override;
    virtual std::string to_string(int pos) const override;
    virtual void from_string(int pos, const std::string& str) override;

    // Iterators ---------------------------------------------------------------
public:
    virtual iterator begin() override;
    virtual iterator end() override;
    virtual const_iterator begin() const override;
    virtual const_iterator end() const override;

    // Element access ----------------------------------------------------------
public:
    inline const T& at(int pos) const;
    inline       T& at(int pos);
    inline const T& operator [] (int pos) const;
    inline       T& operator [] (int pos);
    inline const T& front() const;
    inline       T& front();
    inline const T& back() const;
    inline       T& back();
    inline const T* data() const;
    inline       T* data();

    // Capacity ----------------------------------------------------------------
public:
    virtual bool empty() const override;
    virtual int size() const override;
    virtual bool max_size() const override;
    virtual void reserve(int new_cap) override;
    virtual int capacity() const override;
    virtual void shrink_to_fit() override;

    // Modifiers ---------------------------------------------------------------
public:
    virtual void clear() override;
    virtual void insert(iterator pos, const_iterator first, const_iterator last) override;
    virtual void erase(index_iterator pos) override;
    virtual void erase(index_iterator first, index_iterator last) override;
    virtual void push_back() override;
    virtual void emplace_back() override;
    virtual void pop_back() override;
    virtual void resize(int count) override;
    virtual void swap(abstract_vector& other) override;
    virtual void swap(index_iterator pos1, index_iterator pos2) override;
    virtual void set(index_iterator target, index_iterator source) override;

    // Inheritance -------------------------------------------------------------
public:
    virtual bool match(const abstract_vector& other) const override;

    // Data --------------------------------------------------------------------
public:
    inline const std::vector<T,Allocator>& vector() const;
    inline       std::vector<T,Allocator>& vector();

    inline const std::shared_ptr<std::vector<T,Allocator>>& ptr() const;
    inline       std::shared_ptr<std::vector<T,Allocator>>& ptr();

protected:
    std::shared_ptr<std::vector<T,Allocator>> m_vector;
};

} // namespace pdpc

#include <PDPC/Common/Containers/internal/AbstractVector.inl>
