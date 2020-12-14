#pragma once

#include <PDPC/Common/internal/OptionArgument.h>

#include <sstream>
#include <vector>

namespace pdpc {

template<typename T>
class OptionVectorArgumentT final : public OptionArgument
{
    friend class Option;

public:
    OptionVectorArgumentT<T>& set_required();
    OptionVectorArgumentT<T>& set_brief(const std::string& brief);
//    OptionVectorArgumentT<T>& set_default(T value);

    operator std::vector<T>() const;

    std::string to_string() const override;

protected:
    OptionVectorArgumentT(const std::string& id,
                          const std::string& short_id = "");

//    void set_value(const T& value);
    void add(const std::string& str);

//    template<typename U>
//    static std::string to_string(const U& value);
//    static std::string to_string(const std::string& value);

protected:
    std::vector<T> m_values;
};

} // namespace pdpc

#include <PDPC/Common/internal/OptionVectorArgumentT.hpp>
