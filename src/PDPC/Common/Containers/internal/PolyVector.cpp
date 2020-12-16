#include <PDPC/Common/Containers/internal/PolyVector.h>

namespace pdpc {

poly_vector_view<true>::poly_vector_view(const poly_vector* poly_vector, int index) :
    m_poly_vector(poly_vector),
    m_index(index)
{
}

int poly_vector_view<true>::index() const
{
    return m_index;
}

int& poly_vector_view<true>::index()
{
    return m_index;
}

const poly_vector* poly_vector_view<true>::get_poly_vector() const
{
    return m_poly_vector;
}

// =============================================================================

poly_vector_view<false>::poly_vector_view(poly_vector* poly_vector, int index) :
    m_poly_vector(poly_vector),
    m_index(index)
{
}

poly_vector_view<false>::poly_vector_view(const poly_vector_view<false>& other) :
    m_poly_vector(other.m_poly_vector),
    m_index(other.m_index)
{
    std::cout << "cpy ctor:  " << m_index << " (" << m_poly_vector->at<int>(0, m_index) << ")" << std::endl;

    // deep copy
    m_poly_vector->set(m_index, other.m_index);

    std::cout << "vec = ";
    for(int i=0; i<m_poly_vector->size(); ++i)
        std::cout << m_poly_vector->at<int>(0, i) << " ";
    std::cout << std::endl;
}

poly_vector_view<false>& poly_vector_view<false>::operator=(const poly_vector_view<false>& other)
{
    std::cout << "operator=:  "
              << m_index << " (" << m_poly_vector->at<int>(0, m_index) << ")    =    "
              << other.m_index << " (" << other.m_poly_vector->at<int>(0, m_index) << ")"
              << std::endl;

    // deep copy
    m_poly_vector->set(m_index, other.m_index);

    std::cout << "vec = ";
    for(int i=0; i<m_poly_vector->size(); ++i)
        std::cout << m_poly_vector->at<int>(0, i) << " ";
    std::cout << std::endl;
    return *this;
}

int poly_vector_view<false>::index() const
{
    return m_index;
}

int& poly_vector_view<false>::index()
{
    return m_index;
}

const poly_vector* poly_vector_view<false>::get_poly_vector() const
{
    return m_poly_vector;
}

poly_vector* poly_vector_view<false>::get_poly_vector()
{
    return m_poly_vector;
}

void swap(poly_vector_view<false> a, poly_vector_view<false> b)
{
    std::cout << "swap:  "
              << a.index() << " (" << a.get_poly_vector()->at<int>(0, a.index()) << ")    =    "
              << b.index() << " (" << b.get_poly_vector()->at<int>(0, b.index()) << ")"
              << std::endl;

    // deep swap
    a.get_poly_vector()->swap(a.index(), b.index());

    std::cout << "vec = ";
    for(int i=0; i<a.get_poly_vector()->size(); ++i)
        std::cout << a.get_poly_vector()->at<int>(0, i) << " ";
    std::cout << std::endl;
}

// =============================================================================

// Constructor -----------------------------------------------------------------

poly_vector::poly_vector() :
    m_vectors(),
    m_size(0)
{
}

poly_vector::poly_vector(const poly_vector& other) :
    m_vectors(other.m_vectors.size()),
    m_size(other.size())
{
    for(int idx=0; idx<other.count(); ++idx)
    {
        m_vectors[idx].reset(other.m_vectors[idx]->clone());
    }
}

poly_vector::poly_vector(poly_vector&& other) :
    m_vectors(std::move(other.m_vectors)),
    m_size(other.size())
{
}

poly_vector::poly_vector(int size) :
    m_vectors(),
    m_size(size)
{
}

poly_vector::~poly_vector()
{
}

bool poly_vector::match(const poly_vector& other) const
{
    if(this->count() != other.count()) return false;

    for(int idx=0; idx<other.count(); ++idx)
    {
        if((m_vectors[idx] == nullptr && other.m_vectors[idx] != nullptr) ||
           (m_vectors[idx] != nullptr && other.m_vectors[idx] == nullptr) ||
           (!m_vectors[idx]->match(*other.m_vectors[idx])               ) )
        {
            return false;
        }
    }
    return true;
}

void poly_vector::transfer_from(const poly_vector& other)
{
    this->discard_all();
    m_vectors.resize(other.count());

    for(int idx=0; idx<other.count(); ++idx)
    {
        if(other.m_vectors[idx])
        {
            m_vectors[idx].reset(other.m_vectors[idx]->create());
        }
    }
    m_size = 0;
}
void poly_vector::assign(const_iterator first, const_iterator last)
{
    int idx = 0;
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->assign(first[idx], last[idx]);
        }
        ++idx;
    }
    m_size = last - first;
}

poly_vector& poly_vector::operator = (const poly_vector& other)
{
    this->discard_all();
    m_vectors.resize(other.count());

    for(int idx=0; idx<other.count(); ++idx)
    {
        m_vectors[idx].reset(other.m_vectors[idx]->clone());
    }

    m_size = other.m_size;
    return *this;
}

poly_vector& poly_vector::operator = (poly_vector&& other)
{
    this->discard_all();
    m_vectors = std::move(other.m_vectors);

    m_size = other.m_size;
    return *this;
}

void poly_vector::clean()
{
    this->discard_all();
}

// IO --------------------------------------------------------------------------

std::ostream& poly_vector::write(std::ostream& os) const
{
    int count = m_vectors.size();

    IO::write_value(os, m_size);
    IO::write_value(os, count);
    for(int idx=0; idx<this->count(); ++idx)
    {
        bool is_null = m_vectors[idx] == nullptr;
        IO::write_value(os, is_null);
        if(!is_null)
        {
            IO::TypeID id = m_vectors[idx]->type_id();
            IO::write_value(os, id);
            m_vectors[idx]->write(os);
        }
    }
    return os;
}

std::istream& poly_vector::read(std::istream& is)
{
    this->discard_all();
    int count = -1;

    IO::read_value(is, m_size);
    IO::read_value(is, count);

    m_vectors.resize(count);
    for(int idx=0; idx<this->count(); ++idx)
    {
        bool is_null = true;
        IO::read_value(is, is_null);
        if(!is_null)
        {
            IO::TypeID id = IO::ID_INVALID;
            IO::read_value(is, id);
            m_vectors[idx].reset(abstract_vector::create(id));
            m_vectors[idx]->read(is);
        }
    }
    return is;
}

// Capacity --------------------------------------------------------------------

void poly_vector::reserve(int new_cap)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->reserve(new_cap);
        }
    }
}

void poly_vector::shrink_to_fit()
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->shrink_to_fit();
        }
    }
}


// Modifiers -------------------------------------------------------------------

void poly_vector::clear()
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->clear();
        }
    }
    m_size = 0;
}

void poly_vector::insert(const iterator& pos, const const_iterator& first, const const_iterator& last)
{
    PDPC_DEBUG_ASSERT(pos.match(first));
    PDPC_DEBUG_ASSERT(pos.match(last));

    int idx = 0;
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->insert(pos[idx], first[idx], last[idx]);
        }
        ++idx;
    }
    m_size += last - first;
}

void poly_vector::erase(int pos)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->erase(pos);
        }
    }
    --m_size;
}

void poly_vector::erase(int first, int last)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->erase(first, last);
        }
    }
    m_size -= last-first;
}

void poly_vector::push_back()
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->push_back();
        }
    }
    ++m_size;
}

void poly_vector::emplace_back()
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->emplace_back();
        }
    }
    ++m_size;
}

void poly_vector::pop_back()
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->pop_back();
        }
    }
    --m_size;
}

void poly_vector::resize(int size)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->resize(size);
        }
    }
    m_size = size;
}

void poly_vector::swap(int i, int j)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->swap(i, j);
        }
    }
}

void poly_vector::set(int target, int source)
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            v->set(target, source);
        }
    }
}

// Checking --------------------------------------------------------------------

bool poly_vector::has(int idx) const
{
    return (idx < this->count()) && (m_vectors[idx]);
}

bool poly_vector::_is_valid() const
{
    for(auto& v : m_vectors)
    {
        if(v)
        {
            if(v->size() != m_size)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
    }
    return true;
}

} // namespace pdpc
