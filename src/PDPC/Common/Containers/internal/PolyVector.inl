#include <PDPC/Common/Containers/internal/PolyVector.h>

namespace pdpc {

template<bool is_const>
int distance(poly_vector_iterator<is_const> a, poly_vector_iterator<is_const> b)
{
    return b - a;
}

template<bool is_const>
poly_vector_iterator<is_const>::poly_vector_iterator(vector* v, int n) :
    m_poly_vector(v),
    m_iterators(v->count()),
    m_current(n)
{
    for(int idx=0; idx<int(v->count()); ++idx)
    {
        if(v->m_vectors[idx])
        {
            m_iterators[idx] = v->operator[](idx)->begin() + n;
        }
    }
}

template<bool is_const>
template<bool is_other_const>
bool poly_vector_iterator<is_const>::match(const poly_vector_iterator<is_other_const>& other) const
{
    if(this->count() != other.count()) return false;

    for(int idx=0; idx<this->count(); ++idx)
    {
        if(!m_iterators[idx].match(other[idx]))
        {
            return false;
        }
    }
    return true;
}

template<bool is_const>
int poly_vector_iterator<is_const>::count() const
{
    return m_iterators.size();
}

template<bool is_const>
poly_vector_iterator<is_const>& poly_vector_iterator<is_const>::operator ++ ()
{
    for(auto& it : m_iterators)
    {
        ++it;
    }
    ++m_current;
    return *this;
}

template<bool is_const>
poly_vector_iterator<is_const>& poly_vector_iterator<is_const>::operator -- ()
{
    for(auto& it : m_iterators)
    {
        --it;
    }
    --m_current;
    return *this;
}

template<bool is_const>
bool poly_vector_iterator<is_const>::operator != (const self& other) const
{
    return m_current != other.m_current;
}

template<bool is_const>
bool poly_vector_iterator<is_const>::operator == (const self& other) const
{
    return m_current == other.m_current;
}

template<bool is_const>
bool poly_vector_iterator<is_const>::operator < (const self& other) const
{
    return m_current < other.m_current;
}

template<bool is_const>
int poly_vector_iterator<is_const>::operator - (const self& other) const
{
    return m_current - other.m_current;
}

template<bool is_const>
poly_vector_iterator<is_const> poly_vector_iterator<is_const>::operator + (int i) const
{
    poly_vector_iterator cpy(*this);
    for(auto& it : cpy.m_iterators)
    {
        it += i;
    }
    cpy.m_current += i;
    return cpy;
}

template<bool is_const>
poly_vector_iterator<is_const> poly_vector_iterator<is_const>::operator - (int i) const
{
    return *this + (-i);
}

template<bool is_const>
poly_vector_view<is_const> poly_vector_iterator<is_const>::operator * ()
{
    return poly_vector_view<is_const>(m_poly_vector, m_current);
}

template<bool is_const>
const typename poly_vector_iterator<is_const>::iterator& poly_vector_iterator<is_const>::operator [](int idx) const
{
    return m_iterators[idx];
}

template<bool is_const>
typename poly_vector_iterator<is_const>::iterator& poly_vector_iterator<is_const>::operator [](int idx)
{
    return m_iterators[idx];
}

// =============================================================================

int poly_vector::count() const
{
    return m_vectors.size();
}

// Iterator --------------------------------------------------------------------

poly_vector::iterator poly_vector::begin()
{
    return iterator(this, 0);
}

poly_vector::iterator poly_vector::end()
{
    return iterator(this, m_size);
}

poly_vector::const_iterator poly_vector::cbegin()
{
    return const_iterator(this, 0);
}

poly_vector::const_iterator poly_vector::cend()
{
    return const_iterator(this, m_size);
}

poly_vector::const_iterator poly_vector::begin() const
{
    return const_iterator(this, 0);
}

poly_vector::const_iterator poly_vector::end() const
{
    return const_iterator(this, m_size);
}

// Capacity --------------------------------------------------------------------

bool poly_vector::empty() const
{
    return m_size == 0;
}

int poly_vector::size() const
{
    return m_size;
}

// Add/Remove ------------------------------------------------------------------

template<typename T, class A>
int poly_vector::add()
{
    auto it = std::find(m_vectors.begin(), m_vectors.end(), nullptr);
    int idx = std::distance(m_vectors.begin(), it);
    if(it == m_vectors.end())
    {
        m_vectors.emplace_back(new concrete_vector<T,A>(m_size));
    }
    else
    {
        m_vectors[idx].reset(new concrete_vector<T,A>(m_size));
    }
    return idx;
}

int poly_vector::add(IO::TypeID type)
{
    auto it = std::find(m_vectors.begin(), m_vectors.end(), nullptr);
    int idx = std::distance(m_vectors.begin(), it);
    if(it == m_vectors.end())
    {
        m_vectors.emplace_back(abstract_vector::create(type, m_size));
    }
    else
    {
        m_vectors[idx].reset(abstract_vector::create(type, m_size));
    }
    return idx;
}

template<typename T, class A>
void poly_vector::discard(int idx)
{
    PDPC_DEBUG_ASSERT((this->has<T,A>(idx)));
    m_vectors[idx].reset(nullptr);
}

void poly_vector::discard(int idx)
{
    PDPC_DEBUG_ASSERT((this->has(idx)));
    m_vectors[idx].reset(nullptr);
}

void poly_vector::discard_all()
{
    m_vectors.clear();
}

// Accessors -------------------------------------------------------------------

template<typename T, class A>
const T& poly_vector::at(int idx, int n) const
{
    PDPC_DEBUG_ASSERT(0 <= n && n < int(this->at<T,A>(idx).size()));
    return this->at<T,A>(idx)[n];
}

template<typename T, class A>
T& poly_vector::at(int idx, int n)
{
    PDPC_DEBUG_ASSERT(0 <= n && n < int(this->at<T,A>(idx).size()));
    return this->at<T,A>(idx)[n];
}

template<typename T, class A>
const std::vector<T,A>& poly_vector::at(int idx) const
{
    return this->get<T,A>(idx).vector();
}

template<typename T, class A>
std::vector<T,A>& poly_vector::at(int idx)
{
    return this->get<T,A>(idx).vector();
}

template<typename T, class A>
const std::shared_ptr<std::vector<T,A>>& poly_vector::ptr(int idx) const
{
    return this->get<T,A>(idx).ptr();
}

template<typename T, class A>
std::shared_ptr<std::vector<T,A>>& poly_vector::ptr(int idx)
{
    return this->get<T,A>(idx).ptr();
}

const abstract_vector* poly_vector::at(int idx) const
{
    PDPC_DEBUG_ASSERT(0 <= idx && idx < int(m_vectors.size()));
    return m_vectors[idx].get();
}

// Checking --------------------------------------------------------------------

template<typename T, class A>
bool poly_vector::has(int idx) const
{
    return (idx < this->count()) && (m_vectors[idx]) && (m_vectors[idx]->is<T,A>());
}

// Internal --------------------------------------------------------------------

template<typename T, class A>
const concrete_vector<T,A>& poly_vector::get(int idx) const
{
    PDPC_DEBUG_ASSERT((this->has<T,A>(idx)));
    return m_vectors[idx]->cast<T,A>();
}

template<typename T, class A>
concrete_vector<T,A>& poly_vector::get(int idx)
{
    PDPC_DEBUG_ASSERT((this->has<T,A>(idx)));
    return m_vectors[idx]->cast<T,A>();
}

const abstract_vector* poly_vector::operator [](int idx) const
{
    PDPC_DEBUG_ASSERT((this->has(idx)));
    return m_vectors[idx].get();
}

abstract_vector* poly_vector::operator [](int idx)
{
    PDPC_DEBUG_ASSERT((this->has(idx)));
    return m_vectors[idx].get();
}

// =============================================================================

template<typename T>
const T& poly_vector_view<true>::at(int idx) const
{
    return m_poly_vector->at<T>(idx, m_index);
}

template<typename T>
const T& poly_vector_view<false>::at(int idx) const
{
    return m_poly_vector->at<T>(idx, m_index);
}

template<typename T>
T& poly_vector_view<false>::at(int idx)
{
    return m_poly_vector->at<T>(idx, m_index);
}

} // namespace pdpc
