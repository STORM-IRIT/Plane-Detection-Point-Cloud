#include <PDPC/Common/Containers/internal/AbstractVector.h>

namespace pdpc {

// Accessors -------------------------------------------------------------------

template<typename T>
const T& abstract_vector_view<true>::value() const
{
    return this->cast<T>().value();
}

// Inheritance -----------------------------------------------------------------

template<class T, class A>
concrete_vector_view<true,T,A>& abstract_vector_view<true>::cast()
{
    PDPC_DEBUG_ASSERT((this->is<true,T,A>()));
    return static_cast<concrete_vector_view<true,T,A>&>(*this);
}

template<class T, class A>
const concrete_vector_view<true,T,A>& abstract_vector_view<true>::cast() const
{
    PDPC_DEBUG_ASSERT((this->is<true,T,A>()));
    return static_cast<const concrete_vector_view<true,T,A>&>(*this);
}

template<class T, class A>
bool abstract_vector_view<true>::is() const
{
    return dynamic_cast<const concrete_vector_view<true,T,A>*>(this) != nullptr;
}

// =============================================================================

// Accessors -------------------------------------------------------------------

template<typename T>
const T& abstract_vector_view<false>::value() const
{
    return this->cast<T>().value();
}

template<typename T>
T& abstract_vector_view<false>::value()
{
    return this->cast<T>().value();
}

// Inheritance -----------------------------------------------------------------

template<class T, class A>
concrete_vector_view<false,T,A>& abstract_vector_view<false>::cast()
{
    PDPC_DEBUG_ASSERT((this->is<false,T,A>()));
    return static_cast<concrete_vector_view<false,T,A>&>(*this);
}

template<class T, class A>
const concrete_vector_view<false,T,A>& abstract_vector_view<false>::cast() const
{
    PDPC_DEBUG_ASSERT((this->is<false,T,A>()));
    return static_cast<const concrete_vector_view<false,T,A>&>(*this);
}

template<class T, class A>
bool abstract_vector_view<false>::is() const
{
    return dynamic_cast<const concrete_vector_view<false,T,A>*>(this) != nullptr;
}

// =============================================================================

template<typename T, class A>
concrete_vector_view<true,T,A>::concrete_vector_view(const_iterator it) :
    m_iterator(it)
{
}

// Accessors -------------------------------------------------------------------

template<typename T, class A>
const T& concrete_vector_view<true,T,A>::value() const
{
    return *m_iterator;
}

// =============================================================================

template<typename T, class A>
concrete_vector_view<false,T,A>::concrete_vector_view(iterator it) :
    m_iterator(it)
{
}

// Accessors -------------------------------------------------------------------

template<typename T, class A>
const T& concrete_vector_view<false,T,A>::value() const
{
    return *m_iterator;
}

template<typename T, class A>
T& concrete_vector_view<false,T,A>::value()
{
    return *m_iterator;
}

// Inheritance -----------------------------------------------------------------

template<bool is_const>
template<typename T, class A>
concrete_vector_iterator<is_const,T,A>& abstract_vector_iterator<is_const>::cast()
{
    PDPC_DEBUG_ASSERT((this->is<T,A>()));
    return static_cast<concrete_vector_iterator<is_const,T,A>&>(*this);
}

template<bool is_const>
template<typename T, class A>
const concrete_vector_iterator<is_const,T,A>& abstract_vector_iterator<is_const>::cast() const
{
    PDPC_DEBUG_ASSERT((this->is<T,A>()));
    return static_cast<const concrete_vector_iterator<is_const,T,A>&>(*this);
}

template<bool is_const>
template<typename T, class A>
bool abstract_vector_iterator<is_const>::is() const
{
    return dynamic_cast<const concrete_vector_iterator<is_const,T,A>*>(this) != nullptr;
}

// =============================================================================

template<bool is_const, typename T, class A>
concrete_vector_iterator<is_const,T,A>::concrete_vector_iterator(iterator it) :
    base(),
    m_view(it)
{
}

template<bool is_const, typename T, class A>
concrete_vector_iterator<is_const,T,A>::concrete_vector_iterator(const concrete_vector_iterator<is_const,T,A>& other) :
    base(),
    m_view(other.m_view.m_iterator)
{
}

template<bool is_const, typename T, class A>
abstract_vector_iterator<is_const>* concrete_vector_iterator<is_const,T,A>::clone() const
{
    return new concrete_vector_iterator<is_const,T,A>(*this);
}

template<bool is_const, typename T, class A>
bool concrete_vector_iterator<is_const,T,A>::match(const abstract_vector_iterator<true>& other) const
{
    return other.is<T,A>();
}

template<bool is_const, typename T, class A>
bool concrete_vector_iterator<is_const,T,A>::match(const abstract_vector_iterator<false>& other) const
{
    return other.is<T,A>();
}

template<bool is_const, typename T, class A>
void concrete_vector_iterator<is_const,T,A>::increment(int n)
{
    m_view.m_iterator += n;
}

template<bool is_const, typename T, class A>
bool concrete_vector_iterator<is_const,T,A>::equal_to(const base& other) const
{
    return m_view.m_iterator == other.template cast<T,A>().m_view.m_iterator;
}

template<bool is_const, typename T, class A>
bool concrete_vector_iterator<is_const,T,A>::less_than(const base& other) const
{
    return m_view.m_iterator < other.template cast<T,A>().m_view.m_iterator;
}

template<bool is_const, typename T, class A>
int concrete_vector_iterator<is_const,T,A>::substract(const base& other) const
{
    return m_view.m_iterator - other.template cast<T,A>().m_view.m_iterator;
}

template<bool is_const, typename T, class A>
typename concrete_vector_iterator<is_const,T,A>::abstract_view* concrete_vector_iterator<is_const,T,A>::get()
{
    return &m_view;
}

template<bool is_const, typename T, class A>
typename concrete_vector_iterator<is_const,T,A>::iterator& concrete_vector_iterator<is_const,T,A>::get_iterator()
{
    return m_view.m_iterator;
}

// =============================================================================

template<bool is_const>
abstract_vector_iterator_interface<is_const>::abstract_vector_iterator_interface(abstract_vector_iterator<is_const>* ptr) :
    m_iterator(ptr)
{
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>::abstract_vector_iterator_interface(const abstract_vector_iterator_interface& other) :
    m_iterator(other.m_iterator->clone())
{
}

template<bool is_const>
template<bool is_other_const>
bool abstract_vector_iterator_interface<is_const>::match(const abstract_vector_iterator_interface<is_other_const>& other) const
{
    if((m_iterator == nullptr && other.interface() != nullptr) ||
       (m_iterator != nullptr && other.interface() == nullptr) ||
       (!m_iterator->match(*other.interface())))
    {
        return false;
    }
    else
    {
        return true;
    }
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>& abstract_vector_iterator_interface<is_const>::operator = (const abstract_vector_iterator_interface& other)
{
    m_iterator.reset(other.m_iterator->clone());
    return *this;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>& abstract_vector_iterator_interface<is_const>::operator += (int n)
{
    m_iterator->increment(n);
    return *this;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>& abstract_vector_iterator_interface<is_const>::operator -= (int n)
{
    m_iterator->increment(-n);
    return *this;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>& abstract_vector_iterator_interface<is_const>::operator ++ ()
{
    m_iterator->increment(1);
    return *this;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const>& abstract_vector_iterator_interface<is_const>::operator -- ()
{
    m_iterator->increment(-1);
    return *this;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const> abstract_vector_iterator_interface<is_const>::operator + (int n) const
{
    abstract_vector_iterator_interface cpy(*this);
    cpy += n;
    return cpy;
}

template<bool is_const>
abstract_vector_iterator_interface<is_const> abstract_vector_iterator_interface<is_const>::operator - (int n) const
{
    abstract_vector_iterator_interface cpy(*this);
    cpy -= n;
    return cpy;
}

template<bool is_const>
int abstract_vector_iterator_interface<is_const>::operator - (const abstract_vector_iterator_interface& other) const
{
    return m_iterator->substract(*other.m_iterator.get());
}

template<bool is_const>
bool abstract_vector_iterator_interface<is_const>::operator == (const abstract_vector_iterator_interface& other) const
{
    return m_iterator->equal_to(*other.m_iterator.get());
}

template<bool is_const>
bool abstract_vector_iterator_interface<is_const>::operator != (const abstract_vector_iterator_interface& other) const
{
    return ! m_iterator->equal_to(*other.m_iterator.get());
}

template<bool is_const>
bool abstract_vector_iterator_interface<is_const>::operator < (const abstract_vector_iterator_interface& other) const
{
    return m_iterator->less_than(*other.m_iterator.get());
}

template<bool is_const>
typename abstract_vector_iterator_interface<is_const>::abstract_view* abstract_vector_iterator_interface<is_const>::operator * () const
{
    return m_iterator->get();
}

template<bool is_const>
typename abstract_vector_iterator_interface<is_const>::abstract_view* abstract_vector_iterator_interface<is_const>::operator * ()
{
    return m_iterator->get();
}

template<bool is_const>
template<typename T, class A>
typename abstract_vector_iterator_interface<is_const>::template vector_iterator<T,A>& abstract_vector_iterator_interface<is_const>::iterator()
{
    return m_iterator->template cast<T,A>().get_iterator();
}

template<bool is_const>
const abstract_vector_iterator<is_const>* abstract_vector_iterator_interface<is_const>::interface() const
{
    return m_iterator.get();
}

// =============================================================================

// Constructor -----------------------------------------------------------------

abstract_vector* abstract_vector::create(IO::TypeID type)
{
    return create(type, 0);
}

abstract_vector* abstract_vector::create(IO::TypeID type, int size)
{
    switch(type)
    {
        case IO::ID_BOOL           : PDPC_DEBUG_ASSERT(false); return nullptr; // "bool not yet supported"
        case IO::ID_INT            : return new concrete_vector<int>(size);
        case IO::ID_UINT           : return new concrete_vector<uint>(size);
        case IO::ID_FLOAT          : return new concrete_vector<float>(size);
        case IO::ID_DOUBLE         : warning() << "double -> float"; return new concrete_vector<float>(size); // double are not 'allowed' (this is required for loadgraph with properties...)
        case IO::ID_STRING         : return new concrete_vector<std::string>(size);
        case IO::ID_EIGEN_VECTOR2I : return new concrete_vector<Eigen::Vector2i>(size);
        case IO::ID_EIGEN_VECTOR3I : return new concrete_vector<Eigen::Vector3i>(size);
        case IO::ID_EIGEN_VECTOR4I : return new concrete_vector<Eigen::Vector4i>(size);
        case IO::ID_EIGEN_VECTOR2UI: return new concrete_vector<Eigen::Matrix<uint,2,1>>(size);
        case IO::ID_EIGEN_VECTOR3UI: return new concrete_vector<Eigen::Matrix<uint,3,1>>(size);
        case IO::ID_EIGEN_VECTOR4UI: return new concrete_vector<Eigen::Matrix<uint,4,1>>(size);
        case IO::ID_EIGEN_VECTOR2F : return new concrete_vector<Eigen::Vector2f>(size);
        case IO::ID_EIGEN_VECTOR3F : return new concrete_vector<Eigen::Vector3f>(size);
        case IO::ID_EIGEN_VECTOR4F : return new concrete_vector<Eigen::Vector4f>(size);
        case IO::ID_EIGEN_VECTOR2D : return new concrete_vector<Eigen::Vector2d>(size);
        case IO::ID_EIGEN_VECTOR3D : return new concrete_vector<Eigen::Vector3d>(size);
        case IO::ID_EIGEN_VECTOR4D : return new concrete_vector<Eigen::Vector4d>(size);
        default: return nullptr;
    }
}

// Iterators -------------------------------------------------------------------

abstract_vector::index_iterator abstract_vector::index_begin() const
{
    return index_iterator(0);
}

abstract_vector::index_iterator abstract_vector::index_end() const
{
    return index_iterator(this->size());
}

// Inheritance -----------------------------------------------------------------

template<class T, class A>
concrete_vector<T,A>& abstract_vector::cast()
{
    PDPC_DEBUG_ASSERT((this->is<T,A>()));
    return static_cast<concrete_vector<T,A>&>(*this);
}

template<class T, class A>
const concrete_vector<T,A>& abstract_vector::cast() const
{
    PDPC_DEBUG_ASSERT((this->is<T,A>()));
    return static_cast<const concrete_vector<T,A>&>(*this);
}

template<class T, class A>
bool abstract_vector::is() const
{
    return dynamic_cast<const concrete_vector<T,A>*>(this) != nullptr;
}

// Constructors ----------------------------------------------------------------

template<class T, class A>
concrete_vector<T,A>::concrete_vector() :
    m_vector(std::make_shared<std::vector<T,A>>())
{
}

template<class T, class A>
concrete_vector<T,A>::concrete_vector(int size) :
    m_vector(std::make_shared<std::vector<T,A>>(size))
{
}

template<class T, class A>
concrete_vector<T,A>::concrete_vector(const concrete_vector<T,A>& other) :
    m_vector(std::make_shared<std::vector<T,A>>(other.vector()))
{
}

template<class T, class A>
void concrete_vector<T,A>::assign(const_iterator first, const_iterator last)
{
    m_vector->assign(first.iterator<T,A>(), last.iterator<T,A>());
}

// Clone/Create ----------------------------------------------------------------

template<class T, class A>
abstract_vector* concrete_vector<T,A>::clone() const
{
    return new concrete_vector<T,A>(*this);
}

template<class T, class A>
abstract_vector* concrete_vector<T,A>::create() const
{
    return new concrete_vector<T,A>(0);
}

template<class T, class A>
abstract_vector* concrete_vector<T,A>::create(int size) const
{
    return new concrete_vector<T,A>(size);
}

// IO --------------------------------------------------------------------------

template<class T, class A>
IO::TypeID concrete_vector<T,A>::type_id() const
{
    return IO::type_id<T>();
}

template<class T, class A>
std::ostream& concrete_vector<T,A>::write(std::ostream& os) const
{
    int size = this->size();
    os.write(reinterpret_cast<char*>(&size), sizeof(int));
    os.write(reinterpret_cast<const char*>(m_vector->data()), size*sizeof(T));
    return os;
}

template<class T, class A>
std::istream& concrete_vector<T,A>::read(std::istream& is)
{
    int size = -1;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));
    this->resize(size);
    is.read(reinterpret_cast<char*>(m_vector->data()), size*sizeof(T));
    return is;
}

template<class T, class A>
std::string concrete_vector<T,A>::to_string(int pos) const
{
    return IO::to_string(m_vector->operator[](pos));
}

template<class T, class A>
void concrete_vector<T,A>::from_string(int pos, const std::string& str)
{
    IO::from_string(m_vector->operator[](pos), str);
}


// Iterators -------------------------------------------------------------------

template<class T, class A>
typename concrete_vector<T,A>::iterator concrete_vector<T,A>::begin()
{
    return iterator(new concrete_vector_iterator<false,T,A>(m_vector->begin()));
}

template<class T, class A>
typename concrete_vector<T,A>::iterator concrete_vector<T,A>::end()
{
    return iterator(new concrete_vector_iterator<false,T,A>(m_vector->end()));
}

template<class T, class A>
typename concrete_vector<T,A>::const_iterator concrete_vector<T,A>::begin() const
{
    return const_iterator(new concrete_vector_iterator<true,T,A>(m_vector->begin()));
}

template<class T, class A>
typename concrete_vector<T,A>::const_iterator concrete_vector<T,A>::end() const
{
    return const_iterator(new concrete_vector_iterator<true,T,A>(m_vector->end()));
}

// Element access --------------------------------------------------------------

template<class T, class A>
const T& concrete_vector<T,A>::at(int pos) const
{
    return m_vector->at(pos);
}

template<class T, class A>
T& concrete_vector<T,A>::at(int pos)
{
    return m_vector->at(pos);
}

template<class T, class A>
const T& concrete_vector<T,A>::operator [] (int pos) const
{
    return m_vector->operator[](pos);
}

template<class T, class A>
T& concrete_vector<T,A>::operator [] (int pos)
{
    return m_vector->operator[](pos);
}

template<class T, class A>
const T& concrete_vector<T,A>::front() const
{
    return m_vector->front();
}

template<class T, class A>
T& concrete_vector<T,A>::front()
{
    return m_vector->front();
}

template<class T, class A>
const T& concrete_vector<T,A>::back() const
{
    return m_vector->back();
}

template<class T, class A>
T& concrete_vector<T,A>::back()
{
    return m_vector->back();
}

template<class T, class A>
const T* concrete_vector<T,A>::data() const
{
    return m_vector->data();
}

template<class T, class A>
T* concrete_vector<T,A>::data()
{
    return m_vector->data();
}

// Capacity --------------------------------------------------------------------

template<typename T, class A>
bool concrete_vector<T,A>::empty() const
{
    return m_vector->empty();
}

template<typename T, class A>
int concrete_vector<T,A>::size() const
{
    return m_vector->size();
}

template<typename T, class A>
bool concrete_vector<T,A>::max_size() const
{
    return m_vector->max_size();
}

template<typename T, class A>
void concrete_vector<T,A>::reserve(int new_cap)
{
    m_vector->reserve(new_cap);
}

template<typename T, class A>
int concrete_vector<T,A>::capacity() const
{
    return m_vector->capacity();
}

template<typename T, class A>
void concrete_vector<T,A>::shrink_to_fit()
{
    m_vector->shrink_to_fit();
}
// Modifiers -------------------------------------------------------------------

template<typename T, class A>
void concrete_vector<T,A>::clear()
{
    m_vector->clear();
}

template<typename T, class A>
void concrete_vector<T,A>::insert(iterator pos, const_iterator first, const_iterator last)
{
    m_vector->insert(pos.iterator<T,A>(), first.iterator<T,A>(), last.iterator<T,A>());
}

template<typename T, class A>
void concrete_vector<T,A>::erase(index_iterator pos)
{
    m_vector->erase(m_vector->begin() + pos.index());
}

template<typename T, class A>
void concrete_vector<T,A>::erase(index_iterator first, index_iterator last)
{
    m_vector->erase(m_vector->begin() + first.index(),
                    m_vector->begin() + last.index());
}

template<typename T, class A>
void concrete_vector<T,A>::push_back()
{
    m_vector->push_back(T());
}

template<typename T, class A>
void concrete_vector<T,A>::emplace_back()
{
    m_vector->emplace_back();
}

template<typename T, class A>
void concrete_vector<T,A>::pop_back()
{
    m_vector->pop_back();
}

template<typename T, class A>
void concrete_vector<T,A>::resize(int count)
{
    m_vector->resize(count);
}

template<typename T, class A>
void concrete_vector<T,A>::swap(abstract_vector& other)
{
    m_vector->swap(other.cast<T,A>().vector());
}

template<typename T, class A>
void concrete_vector<T,A>::swap(index_iterator pos1, index_iterator pos2)
{
    std::swap(m_vector->operator[](pos1.index()),
              m_vector->operator[](pos2.index()));
}

template<typename T, class A>
void concrete_vector<T,A>::set(index_iterator target, index_iterator source)
{
    m_vector->operator[](target.index()) = m_vector->operator[](source.index());
}

// Inheritance -----------------------------------------------------------------

template<typename T, class A>
bool concrete_vector<T,A>::match(const abstract_vector& other) const
{
    return other.is<T,A>();
}

// Data ------------------------------------------------------------------------

template<typename T, class A>
const std::vector<T,A>& concrete_vector<T,A>::vector() const
{
    return *m_vector.get();
}

template<typename T, class A>
std::vector<T,A>& concrete_vector<T,A>::vector()
{
    return *m_vector.get();
}

template<typename T, class A>
const std::shared_ptr<std::vector<T,A>>& concrete_vector<T,A>::ptr() const
{
    return m_vector;
}

template<typename T, class A>
std::shared_ptr<std::vector<T,A>>& concrete_vector<T,A>::ptr()
{
    return m_vector;
}

} // namespace pdpc
