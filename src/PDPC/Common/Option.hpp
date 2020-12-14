#include <PDPC/Common/Option.h>

namespace pdpc {

template<typename T>
typename Option::Arg<T>& Option::get(const String& id, const String& id2)
{
    m_args.emplace_back(new Arg<T>(id, id2));
    auto& a = static_cast<Arg<T>&>(*m_args.back());

    auto it = std::find_if(m_input.begin(), m_input.end(), [&a](const auto& str)
    {
        return a.match(str);
    });
    const int idx = std::distance(m_input.begin(), it);

    if(int(m_input.size()) <= idx) return a;

    if(idx == int(m_input.size()-1) /*||
       (!m_input[idx+1].empty() && m_input[idx+1].front() == '-')*/)
    {
        std::cout << "Warning: missing argument after -" << a.id() << std::endl;
    }
    else
    {
        a.set_found();
        a.set(m_input[idx+1]);
    }

    return a;
}

template<typename T>
typename Option::ArgVec<T>& Option::get_vec(const String& id, const String& id2)
{
    m_args.emplace_back(new ArgVec<T>(id, id2));
    auto& a = static_cast<ArgVec<T>&>(*m_args.back());

    auto it = std::find_if(m_input.begin(), m_input.end(), [&a](const auto& str)
    {
        return a.match(str);
    });
    const int idx = std::distance(m_input.begin(), it);

    if(int(m_input.size()) <= idx) return a;

    if(idx == int(m_input.size()-1) /*||
       (!m_input[idx+1].empty() && m_input[idx+1].front() == '-')*/)
    {
        std::cout << "Warning: missing argument after -" << a.id() << std::endl;
    }
    else
    {
        a.set_found();
        for(int i=idx+1; i<int(m_input.size()) && !is_arg(m_input[i]); ++i)
        {
            a.add(m_input[i]);
        }
    }

    return a;
}

template<typename T>
typename Option::ArgVec<T>& Option::get_delimited_vec(const String& id, const String& id2)
{
    m_args.emplace_back(new ArgVec<T>(id, id2));
    auto& a = static_cast<ArgVec<T>&>(*m_args.back());

    auto it = std::find_if(m_input.begin(), m_input.end(), [&a](const auto& str)
    {
        return a.match(str);
    });
    const int idx = std::distance(m_input.begin(), it);

    if(int(m_input.size()) <= idx) return a;

    if(idx == int(m_input.size()-1) /*||
       (!m_input[idx+1].empty() && m_input[idx+1].front() == '-')*/)
    {
        std::cout << "Warning: missing argument after -" << a.id() << std::endl;
    }
    else
    {
        if(!is_begin_delimiter(m_input[idx+1]))
        {
            std::cout << "Warning: missing begin delimiter after -" << a.id() << std::endl;
        }
        else if(std::find_if(m_input.begin()+idx, m_input.end(), [](const auto& str){return is_end_delimiter(str);}) == m_input.end())
        {
            std::cout << "Warning: missing end delimiter after -" << a.id() << std::endl;
        }
        else
        {
            a.set_found();
            for(int i=idx+2; i<int(m_input.size()) && !is_end_delimiter(m_input[i]); ++i)
            {
                a.add(m_input[i]);
            }
        }
    }

    return a;
}

} // namespace pdpc
