#pragma once

#include <PDPC/Common/internal/OptionArgumentT.h>
#include <PDPC/Common/internal/OptionVectorArgumentT.h>

#include <algorithm>
#include <vector>
#include <memory>
#include <iostream>

namespace pdpc {

using String = std::string;

class Option
{
protected:
    template<typename T> using Arg    = OptionArgumentT<T>;
    template<typename T> using ArgVec = OptionVectorArgumentT<T>;

public:
    Option(int argc, char** argv);

    bool operator !() const;
    bool ok() const;
    friend std::ostream& operator <<(std::ostream& os, const Option& opt);

public:
    bool has(const std::string& id, const std::string& id2) const;
    int  index(const std::string& id, const std::string& id2) const;

    const std::string& operator [](int idx) const;

public:
    Arg<bool>&   get_bool(  const String& id, const String& id2 = "");
    Arg<int>&    get_int(   const String& id, const String& id2 = "");
    Arg<float>&  get_float( const String& id, const String& id2 = "");
    Arg<String>& get_string(const String& id, const String& id2 = "");

    ArgVec<String>& get_strings(const String& id, const String& id2 = "");
    ArgVec<int>&    get_ints(const String& id, const String& id2 = "");
    ArgVec<float>&  get_floats(const String& id, const String& id2 = "");

    template<typename T>
    Arg<T>& get(const String& id, const String& id2 = "");

    template<typename T>
    ArgVec<T>& get_vec(const String& id, const String& id2 = "");

    template<typename T>
    ArgVec<T>& get_delimited_vec(const String& id, const String& id2 = "");

public:
    void print_help() const;

    void set_description(const std::string& description);

protected:
    static void cast(const String& str, int& val);
    static void cast(const String& str, float& val);
    static void cast(const String& str, String& val);

    static bool is_arg(const std::string& str);
    static bool is_part_of_number(char c);

    static bool is_begin_delimiter(const std::string& str);
    static bool is_end_delimiter(const std::string& str);

    static bool is_begin_delimiter(char c);
    static bool is_end_delimiter(char c);

protected:
    std::vector<String> m_input;
    std::vector<std::unique_ptr<OptionArgument>> m_args;
    std::string m_description;
};

} // namespace pdpc

#include <PDPC/Common/Option.hpp>
