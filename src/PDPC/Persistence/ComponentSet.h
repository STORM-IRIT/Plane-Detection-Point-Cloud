#pragma once

#include <PDPC/Persistence/Component.h>
#include <PDPC/Common/Containers/PropertyMap.h>

namespace pdpc {

class ComponentSet
{
    // ComponentSet ------------------------------------------------------------
public:
    ComponentSet();
    ComponentSet(int size);

    bool is_valid() const;

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Accessors ---------------------------------------------------------------
public:
    int size() const;
    bool empty() const;

    const Component& component(int i) const;
          Component& component(int i);

    const Component& operator[](int i) const;
          Component& operator[](int i);

    const Component& back() const;
          Component& back();

    const Component& front() const;
          Component& front();

    const std::vector<Component>& data() const;
          std::vector<Component>& data();

    const property_map& properties() const;
          property_map& properties();

    // Modifiers ---------------------------------------------------------------
public:
    void clear();
    void resize(int count);
    void push_back();

    // Data --------------------------------------------------------------------
protected:
    std::vector<Component> m_components;
    property_map           m_properties;

}; // class ComponentSet

} // namespace pdpc
