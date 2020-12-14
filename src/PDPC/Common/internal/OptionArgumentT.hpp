#include <PDPC/Common/internal/OptionArgumentT.h>

namespace pdpc {

template<typename T>
OptionArgumentT<T>& OptionArgumentT<T>::set_required()
{
    m_required = true;
    return *this;
}

template<typename T>
OptionArgumentT<T>& OptionArgumentT<T>::set_brief(const std::string& brief)
{
    m_brief = brief;
    return *this;
}

template<typename T>
OptionArgumentT<T>& OptionArgumentT<T>::set_default(T value)
{
    if(!m_found)
    {
        m_value = value;
    }
    m_default_value = to_string(value);
    return *this;
}

template<typename T>
OptionArgumentT<T>::operator T() const
{
    return m_value;
}

template<typename T>
std::string OptionArgumentT<T>::to_string() const
{
    std::stringstream str;
    str << m_value;
    return str.str();
}

template<typename T>
OptionArgumentT<T>::OptionArgumentT(const std::string& id,
                                    const std::string& short_id) :
    OptionArgument(id, short_id),
    m_value()
{
}

template<typename T>
void OptionArgumentT<T>::set_value(const T& value)
{
    m_value = value;
}

template<typename T>
void OptionArgumentT<T>::set(const std::string& str)
{
    std::stringstream stream(str);
    stream >> m_value;
}

template<typename T>
template<typename U>
std::string OptionArgumentT<T>::to_string(const U& value)
{
    std::stringstream str;
    str << value;
    return str.str();
}

template<typename T>
std::string OptionArgumentT<T>::to_string(const std::string& value)
{
    return value;
}

} // namespace pdpc
