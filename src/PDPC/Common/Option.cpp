#include <PDPC/Common/Option.h>

namespace pdpc {

Option::Option(int argc, char** argv) :
    m_input(argc-1),
    m_args(),
    m_description()
{
    for(int i=0; i<argc-1; ++i)
    {
        m_input[i] = argv[i+1];
    }
}

bool Option::operator !() const
{
    return !ok();
}

bool Option::ok() const
{
    auto& self = *const_cast<Option*>(this);
    bool h = self.get_bool("help", "h").set_brief("Display this help");

    if(h)
    {
        print_help();
        return false;
    }

    bool ok = true;
    for(const auto& arg : m_args)
    {
        if(arg->is_required() && !arg->is_found())
        {
            std::cout << "Error: -" << arg->id() << " is required" << std::endl;
            ok = false;
        }
    }
    return ok;
}

std::ostream& operator << (std::ostream& os, const Option& opt)
{
    int max = 0;
    for(const auto& arg : opt.m_args)
    {
        if(arg->id() != "help")
        {
            max = std::max<int>(max, arg->id().size());
        }
    }

    for(const auto& arg : opt.m_args)
    {
        if(arg->id() != "help")
        {
            os << std::endl;
            os << arg->id();
            for(int i = arg->id().size()-1; i < max; ++i) os << " ";

            os << " = " << arg->to_string();
        }
    }

    return os;
}

bool Option::has(const std::string& id, const std::string& id2) const
{
    for(uint i=0; i<m_input.size(); ++i)
    {
        if(m_input[i] == "-"+id || m_input[i] == "-"+id2)
        {
            return true;
        }
    }
    return false;
}

int Option::index(const std::string& id, const std::string& id2) const
{
    for(uint i=0; i<m_input.size(); ++i)
    {
        if(m_input[i] == "-"+id || m_input[i] == "-"+id2)
        {
            return i;
        }
    }
    return -1;
}

const std::string& Option::operator [](int idx) const
{
    return m_input[idx];
}

template<>
typename Option::Arg<bool>& Option::get<bool>(const String& id, const String& id2)
{
    m_args.emplace_back(new Arg<bool>(id, id2));
    auto& a = static_cast<Arg<bool>&>(*m_args.back());

    auto it = std::find_if(m_input.begin(), m_input.end(), [&a](const auto& str)
    {
        return a.match(str);
    });

    if(it != m_input.end())
    {
        a.set_found();
        a.set_value(true);
    }

    return a;
}

typename Option::Arg<bool>& Option::get_bool(const String& id, const String& id2)
{
    return this->get<bool>(id, id2);
}

typename Option::Arg<int>& Option::get_int(const String& id, const String& id2)
{
    return this->get<int>(id, id2);
}

typename Option::Arg<float>& Option::get_float(const String& id, const String& id2)
{
    return this->get<float>(id, id2);
}

typename Option::Arg<String>& Option::get_string(const String& id, const String& id2)
{
    return this->get<String>(id, id2);
}

typename Option::ArgVec<String>& Option::get_strings(const String& id, const String& id2)
{
    return this->get_vec<String>(id, id2);
}

typename Option::ArgVec<int>& Option::get_ints(const String& id, const String& id2)
{
    return this->get_delimited_vec<int>(id, id2);
}

typename Option::ArgVec<float>& Option::get_floats(const String& id, const String& id2)
{
    return this->get_delimited_vec<float>(id, id2);
}

void Option::print_help() const
{
    if(!m_description.empty())
    {
        std::cout << m_description << std::endl << std::endl;
    }

    int max = 0;
    for(const auto& arg : m_args)
    {
        auto str = "-" + arg->id() + (arg->has_short_id() ? ", -" + arg->short_id() : "");
        max = std::max<int>(max, str.size());
    }

    for(const auto& arg : m_args)
    {
        auto str = "-" + arg->id() + (arg->has_short_id() ? ", -" + arg->short_id() : "");
        std::cout << str;

        for(int i = str.size()-1; i < max+4; ++i) std::cout << " ";

        std::cout << arg->brief();
        if(arg->is_required())
        {
            if(!arg->brief().empty())
                std::cout << " ";
            std::cout << "(REQUIRED)";
        }
        else if(arg->has_default_value())
        {
            if(!arg->brief().empty())
                std::cout << " ";
            std::cout << "(default='" << arg->default_value() << "')";
        }
        std::cout << std::endl;
    }
}

void Option::set_description(const std::string& description)
{
    m_description = description;
}

//! \brief check if a string is an argument and not a negative number
bool Option::is_arg(const std::string& str)
{
    if(str.empty())
    {
        return false;
    }
    else if(str.front() != '-')
    {
        return false;
    }
    else
    {
        const auto content = str.substr(1);
        if(content.empty())
        {
            return false;
        }
        else
        {
            const bool is_number = std::all_of(content.begin(), content.end(), Option::is_part_of_number);
            return !is_number;
        }
    }
}

bool Option::is_part_of_number(char c)
{
    return std::isdigit(c) || c == '.' || c == '-';
}

bool Option::is_begin_delimiter(const std::string& str)
{
    return str.size() == 1 && is_begin_delimiter(str.front());
}

bool Option::is_end_delimiter(const std::string& str)
{
    return str.size() == 1 && is_end_delimiter(str.front());
}

bool Option::is_begin_delimiter(char c)
{
    return c == '(' || c == '[' || c == '{';
}

bool Option::is_end_delimiter(char c)
{
    return c == ')' || c == ']' || c == '}';
}

} // namespace pdpc
