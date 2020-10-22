#pragma once

#include <vector>
#include <algorithm>
#include <functional>

namespace pdpc {

//!
//! \brief The limited_priority_queue class is similar to std::priority_queue
//! but has a limited capacity and handles the comparison differently.
//!
//! In case the capacity is reached, the container is full and push() do not
//! insert a new element if its priority is lower than the current minimal one.
//!
//! The comparison predicate must return true is the first argument has
//! priority on the second one.
//!
//! The element with the highest priority is the last one that is pop out, but
//! is the first one that is iterated through.
//!
//! Example 1:
//! Using std::less as comparison predicate, we have the following situation:
//!
//!     full()      = false
//!     empty()     = false
//!     capacity()  = 6
//!     size()      = 4
//!     top()       = 1
//!     bottom()    = 9
//!
//!     pop() removes the value 9
//!     push(4) adds the value 4
//!
//!     begin            end
//!       v               v
//!     +---+---+---+---+---+---+
//!     | 1 | 3 | 8 | 9 |   |   |
//!     +---+---+---+---+---+---+
//!       ^           ^
//!      top        bottom
//!
//!
//!
//! Example 2:
//! Using std::greater as comparison predicate, we have the following situation:
//!
//!     full()      = true
//!     empty()     = false
//!     capacity()  = 6
//!     size()      = 6
//!     top()       = 9
//!     bottom()    = 2
//!
//!     begin                    end
//!       v                       v
//!     +---+---+---+---+---+---+
//!     | 9 | 8 | 6 | 4 | 3 | 2 |
//!     +---+---+---+---+---+---+
//!       ^                   ^
//!      top                bottom
//!
//!     pop() removes the value 2
//!     push(5) adds the value 5 and remove the value 2
//!     push(0) do nothing
//!
//!
template<class T,
         class CompareT = std::less<T>>
class limited_priority_queue
{
public:
    using value_type      = T;
    using container_type  = std::vector<T>;
    using compare         = CompareT;
    using iterator        = typename container_type::iterator;
    using const_iterator  = typename container_type::const_iterator;
    using this_type       = limited_priority_queue<T,CompareT>;

    // limited_priority_queue --------------------------------------------------
public:
    inline limited_priority_queue();
    inline limited_priority_queue(const this_type& other);
    inline explicit limited_priority_queue(int capacity);
    template<class InputIt>
    inline limited_priority_queue(int capacity, InputIt first, InputIt last);

    inline ~limited_priority_queue();

    inline limited_priority_queue& operator=(const this_type& other);

    // Iterator ----------------------------------------------------------------
public:
    inline iterator       begin();
    inline const_iterator begin() const;
    inline const_iterator cbegin() const;

    inline iterator       end();
    inline const_iterator end() const;
    inline const_iterator cend() const;

    // Element access ----------------------------------------------------------
public:
    inline const T& top() const;
    inline const T& bottom() const;

    inline T& top();
    inline T& bottom();

    // Capacity ----------------------------------------------------------------
public:
    inline bool empty() const;
    inline bool full() const;
    inline int  size() const;
    inline int  capacity() const;

    // Modifiers ---------------------------------------------------------------
public:
    inline bool push(const T& value);
    inline bool push(T&& value);

    inline void pop();

    inline void reserve(int capacity);

    inline void clear();

    // Data --------------------------------------------------------------------
public:
    inline const container_type& container() const;

protected:
    container_type  m_c;
    compare         m_comp;
    int             m_size;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// limited_priority_queue ------------------------------------------------------

template<class T, class Cmp>
limited_priority_queue<T,Cmp>::limited_priority_queue() :
    m_c(),
    m_comp(),
    m_size(0)
{
}


template<class T, class Cmp>
limited_priority_queue<T,Cmp>::limited_priority_queue(const this_type& other) :
    m_c(other.m_c),
    m_comp(other.m_comp),
    m_size(other.m_size)
{
}

template<class T, class Cmp>
limited_priority_queue<T,Cmp>::limited_priority_queue(int capacity) :
    m_c(capacity),
    m_comp(),
    m_size(0)
{
}

template<class T, class Cmp>
template<class InputIt>
limited_priority_queue<T,Cmp>::limited_priority_queue(int capacity, InputIt first, InputIt last) :
    m_c(capacity),
    m_comp(),
    m_size(0)
{
    for(InputIt it=first; it<last; ++it)
    {
        push(*it);
    }
}

template<class T, class Cmp>
limited_priority_queue<T,Cmp>::~limited_priority_queue()
{
}

template<class T, class Cmp>
limited_priority_queue<T,Cmp>& limited_priority_queue<T,Cmp>::operator=(const this_type& other)
{
    m_c    = other.m_c;
    m_comp = other.m_comp;
    m_size = other.m_size;
    return *this;
}

// Iterator --------------------------------------------------------------------

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::iterator limited_priority_queue<T,Cmp>::begin()
{
    return m_c.begin();
}

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::const_iterator limited_priority_queue<T,Cmp>::begin() const
{
    return m_c.begin();
}

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::const_iterator limited_priority_queue<T,Cmp>::cbegin() const
{
    return m_c.cbegin();
}

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::iterator limited_priority_queue<T,Cmp>::end()
{
    return m_c.begin() + m_size;
}

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::const_iterator limited_priority_queue<T,Cmp>::end() const
{
    return m_c.begin() + m_size;
}

template<class T, class Cmp>
typename limited_priority_queue<T,Cmp>::const_iterator limited_priority_queue<T,Cmp>::cend() const
{
    return m_c.cbegin() + m_size;
}

// Element access --------------------------------------------------------------

template<class T, class Cmp>
const T& limited_priority_queue<T,Cmp>::top() const
{
    return m_c[0];
}

template<class T, class Cmp>
const T& limited_priority_queue<T,Cmp>::bottom() const
{
    return m_c[m_size-1];
}

template<class T, class Cmp>
T& limited_priority_queue<T,Cmp>::top()
{
    return m_c[0];
}

template<class T, class Cmp>
T& limited_priority_queue<T,Cmp>::bottom()
{
    return m_c[m_size-1];
}

// Capacity --------------------------------------------------------------------

template<class T, class Cmp>
bool limited_priority_queue<T,Cmp>::empty() const
{
    return m_size == 0;
}

template<class T, class Cmp>
bool limited_priority_queue<T,Cmp>::full() const
{
    return m_size == capacity();
}

template<class T, class Cmp>
int limited_priority_queue<T,Cmp>::size() const
{
    return m_size;
}

template<class T, class Cmp>
int limited_priority_queue<T,Cmp>::capacity() const
{
    return m_c.size();
}

// Modifiers -------------------------------------------------------------------

template<class T, class Cmp>
bool limited_priority_queue<T,Cmp>::push(const T& value)
{
    if(empty())
    {
        if(capacity()>0)
        {
            m_c.front() = value;
            ++m_size;
            return true;
        }
    }
    else
    {
        iterator it = std::upper_bound(begin(), end(), value, m_comp);
        if(it==end())
        {
            if(!full())
            {
                *it = value;
                ++m_size;
                return true;
            }
        }
        else
        {
            if(full())
            {
                std::copy_backward(it, end()-1, end());
                *it = value;
            }
            else
            {
                std::copy_backward(it, end(), end()+1);
                *it = value;
                ++m_size;
            }
            return true;
        }
    }
    return false;
}

template<class T, class Cmp>
bool limited_priority_queue<T,Cmp>::push(T&& value)
{
    if(empty())
    {
        if(capacity()>0)
        {
            m_c.front() = std::move(value);
            ++m_size;
            return true;
        }
    }
    else
    {
        iterator it = std::upper_bound(begin(), end(), std::move(value), m_comp);
        if(it==end())
        {
            if(!full())
            {
                *it = std::move(value);
                ++m_size;
                return true;
            }
        }
        else
        {
            if(full())
            {
                std::copy_backward(it, end()-1, end());
                *it = std::move(value);
            }
            else
            {
                std::copy_backward(it, end(), end()+1);
                *it = std::move(value);
                ++m_size;
            }
            return true;
        }
    }
    return false;
}

template<class T, class Cmp>
void limited_priority_queue<T,Cmp>::pop()
{
    --m_size;
}

template<class T, class Cmp>
void limited_priority_queue<T,Cmp>::reserve(int capacity)
{
    if(m_size>capacity)
    {
        m_size = capacity;
    }
    m_c.resize(capacity);
}

template<class T, class Cmp>
void limited_priority_queue<T,Cmp>::clear()
{
    m_size = 0;
}

// Data ------------------------------------------------------------------------

template<class T, class Cmp>
const typename limited_priority_queue<T,Cmp>::container_type& limited_priority_queue<T,Cmp>::container() const
{
    return m_c;
}

} // namespace pdpc
