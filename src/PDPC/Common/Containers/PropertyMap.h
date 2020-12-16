#pragma once

#include <PDPC/Common/Containers/internal/PolyVector.h>

#include <map>
#include <string>

namespace pdpc {

class property_map : public poly_vector
{
public:
    using String = std::string;
    using poly_vector::at;
    using poly_vector::discard;
    using poly_vector::has;

    // Constructor -------------------------------------------------------------
public:
    property_map();
    property_map(const property_map& other);
    property_map(property_map&& other);
    property_map(int size);
    ~property_map();

    bool match(const property_map& other) const;

    void transfer_from(const property_map& other);

    property_map& operator = (const property_map& other);
    property_map& operator = (property_map&& other);

    void clean();

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Add/Discard -------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline int add(const String& name);
    int add(const String& name, IO::TypeID type);

    void discard(const String& name);
    void discard_all();

    // Accessors ---------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline const T& at(const String& name, int n) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline T& at(const String& name, int n);

    template<typename T, class Allocator = typename allocator<T>::type>
    inline const std::vector<T,Allocator>& at(const String& name) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline std::vector<T,Allocator>& at(const String& name);

    // Id Accessors ------------------------------------------------------------
public:
    int index(const String& name) const;
    String name(int idx) const;

    void rename(const String& from, const String& to);

    // Checking ----------------------------------------------------------------
public:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline bool has(const String& name) const;
    bool has(const String& name) const;

    bool _is_valid() const;

    // Internal ----------------------------------------------------------------
protected:
    template<typename T, class Allocator = typename allocator<T>::type>
    inline const concrete_vector<T,Allocator>& get(const String& name) const;

    template<typename T, class Allocator = typename allocator<T>::type>
    inline concrete_vector<T,Allocator>& get(const String& name);

public:
    const abstract_vector& get(const String& name) const;
          abstract_vector& get(const String& name);

protected:
    std::map<String, int> m_mapID;
    std::vector<String>   m_vecID;

}; // class property_map

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Add/Remove ------------------------------------------------------------------

template<typename T, class A>
int property_map::add(const String& name)
{
    auto it = m_mapID.find(name);

    if(it == m_mapID.end())
    {
        int idx = poly_vector::add<T,A>();

        if(int(m_vecID.size()) < idx+1)
            m_vecID.resize(idx+1);

        m_vecID[idx]  = name;
        m_mapID[name] = idx;

        return idx;
    }
    else
    {
        int idx = it->second;
        PDPC_DEBUG_ASSERT(m_vectors.at(idx)->is<T>());
        return idx;
    }
}

// Accessors -------------------------------------------------------------------

template<typename T, class A>
const T& property_map::at(const String& name, int n) const
{
    return this->at<T,A>(name)[n];
}

template<typename T, class A>
T& property_map::at(const String& name, int n)
{
    return this->at<T,A>(name)[n];
}

template<typename T, class A>
const std::vector<T,A>& property_map::at(const String& name) const
{
    return this->get<T,A>(name).vector();
}

template<typename T, class A>
std::vector<T,A>& property_map::at(const String& name)
{
    return this->get<T,A>(name).vector();
}

// Checking --------------------------------------------------------------------

template<typename T, class A>
bool property_map::has(const String& name) const
{
    auto it = m_mapID.find(name);
    return it != m_mapID.end() && poly_vector::has<T,A>(it->second);
}

// Internal --------------------------------------------------------------------

template<typename T, class A>
const concrete_vector<T,A>& property_map::get(const String& name) const
{
    return poly_vector::get<T,A>(this->index(name));
}

template<typename T, class A>
concrete_vector<T,A>& property_map::get(const String& name)
{
    return poly_vector::get<T,A>(this->index(name));
}

} // namespace pdpc
