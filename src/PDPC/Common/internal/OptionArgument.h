#pragma once

#include <string>

namespace pdpc {

class Option;

class OptionArgument
{
    friend class Option;
    friend std::ostream& operator<<(std::ostream&, const Option&);

public:
    virtual std::string to_string() const = 0;

protected:
    OptionArgument(const std::string& id,
                   const std::string& short_id = "");

public:
    virtual ~OptionArgument(){};

    bool match(const std::string &arg) const;

protected:
    const std::string& id() const;
    const std::string& short_id() const;
    const std::string& brief() const;
    const std::string& default_value();
    bool               is_required() const;
    bool               is_found() const;

protected:
    bool has_id() const;
    bool has_short_id() const;
    bool has_brief() const;
    bool has_default_value();

protected:
    void set_found();

protected:
    std::string m_id;
    std::string m_short_id;
    std::string m_brief;
    std::string m_default_value;
    bool        m_required;
    bool        m_found;
};

} // namespace pdpc
