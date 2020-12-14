#pragma once

#include <PDPC/Common/internal/OptionArgument.h>

#include <sstream>

namespace pdpc {

template<typename T>
class OptionArgumentT final : public OptionArgument
{
    friend class Option;

public:
    OptionArgumentT<T>& set_required();
    OptionArgumentT<T>& set_brief(const std::string& brief);
    OptionArgumentT<T>& set_default(T value);

    operator T() const;

    std::string to_string() const override;

protected:
    OptionArgumentT(const std::string& id,
                    const std::string& short_id = "");

    void set_value(const T& value);
    void set(const std::string& str);

    template<typename U>
    static std::string to_string(const U& value);
    static std::string to_string(const std::string& value);

protected:
    T m_value;
};

} // namespace pdpc

#include <PDPC/Common/internal/OptionArgumentT.hpp>
