#include <PDPC/Common/internal/OptionVectorArgumentT.h>

namespace pdpc {

template<typename T>
OptionVectorArgumentT<T>& OptionVectorArgumentT<T>::set_required()
{
    m_required = true;
    return *this;
}

template<typename T>
OptionVectorArgumentT<T>& OptionVectorArgumentT<T>::set_brief(const std::string& brief)
{
    m_brief = brief;
    return *this;
}

//template<typename T>
//OptionVectorArgumentT<T>& OptionVectorArgumentT<T>::set_default(T value)
//{
//    if(!m_found)
//    {
//        m_value = value;
//    }
//    m_default_value = to_string(value);
//    return *this;
//}

template<typename T>
OptionVectorArgumentT<T>::operator std::vector<T>() const
{
    return m_values;
}

template<typename T>
std::string OptionVectorArgumentT<T>::to_string() const
{
    std::stringstream str;
    for(const auto& val : m_values)
        str << val << " ";
    return str.str();
}

template<typename T>
OptionVectorArgumentT<T>::OptionVectorArgumentT(const std::string& id,
                                                const std::string& short_id) :
    OptionArgument(id, short_id),
    m_values()
{
}

//template<typename T>
//void OptionVectorArgumentT<T>::set_value(const T& value)
//{
//    m_value = value;
//}

template<typename T>
void OptionVectorArgumentT<T>::add(const std::string& str)
{
    std::stringstream stream(str);
    T val;
    stream >> val;
    m_values.push_back(val);
}

//template<typename T>
//template<typename U>
//std::string OptionVectorArgumentT<T>::to_string(const U& value)
//{
//    std::stringstream str;
//    str << value;
//    return str.str();
//}

//template<typename T>
//std::string OptionVectorArgumentT<T>::to_string(const std::string& value)
//{
//    return value;
//}

} // namespace pdpc
