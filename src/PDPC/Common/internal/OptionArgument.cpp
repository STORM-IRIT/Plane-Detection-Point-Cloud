#include <PDPC/Common/internal/OptionArgument.h>

namespace pdpc {

OptionArgument::OptionArgument(const std::string& id,
               const std::string& short_id) :
    m_id(id),
    m_short_id(short_id),
    m_brief(""),
    m_default_value(""),
    m_required(false),
    m_found(false)
{
}

bool OptionArgument::match(const std::string& arg) const
{
    return arg == "-" + m_id || (has_short_id() && arg == "-" + m_short_id);
}

const std::string& OptionArgument::id() const
{
    return m_id;
}

const std::string& OptionArgument::short_id() const
{
    return m_short_id;
}

const std::string& OptionArgument::brief() const
{
    return m_brief;
}

const std::string& OptionArgument::default_value()
{
    return m_default_value;
}

bool OptionArgument::is_required() const
{
    return m_required;
}

bool OptionArgument::is_found() const
{
    return m_found;
}

bool OptionArgument::has_id() const
{
    return !m_id.empty();
}

bool OptionArgument::has_short_id() const
{
    return !m_short_id.empty();
}

bool OptionArgument::has_brief() const
{
    return !m_brief.empty();
}

bool OptionArgument::has_default_value()
{
    return !m_default_value.empty();
}

void OptionArgument::set_found()
{
    m_found = true;
}

} // namespace pdpc
