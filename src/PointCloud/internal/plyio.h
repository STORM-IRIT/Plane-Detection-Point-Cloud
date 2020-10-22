#pragma once

#include <string>
#include <vector>
#include <functional>

namespace plyio {

// String ----------------------------------------------------------------------

std::vector<std::string> to_tokens(const std::string& str);

// Types -----------------------------------------------------------------------

using char_t   = signed char;
using uchar_t  = unsigned char;
using short_t  = int16_t;
using ushort_t = uint16_t;
using int_t    = int32_t;
using uint_t   = uint32_t;
using float_t  = float;
using double_t = double;

enum Type
{
    type_char,
    type_uchar,
    type_short,
    type_ushort,
    type_int,
    type_uint,
    type_float,
    type_double,
    type_unkown
};

Type to_type(const std::string& str);
std::string to_string(Type type);

template<typename T> Type type();
template<> Type type<char_t  >();
template<> Type type<uchar_t >();
template<> Type type<short_t >();
template<> Type type<ushort_t>();
template<> Type type<int_t   >();
template<> Type type<uint_t  >();
template<> Type type<float_t >();
template<> Type type<double_t>();

// Reader ----------------------------------------------------------------------

template<typename T>
using ReadingFunction     = std::function<void(int i, T value)>;
template<typename T>
using ReadingFunctionList = std::function<void(int i, const std::vector<T>& values)>;

using ReadingFunctionChar   = ReadingFunction<char_t>;
using ReadingFunctionUchar  = ReadingFunction<uchar_t>;
using ReadingFunctionShort  = ReadingFunction<short_t>;
using ReadingFunctionUshort = ReadingFunction<ushort_t>;
using ReadingFunctionInt    = ReadingFunction<int_t>;
using ReadingFunctionUint   = ReadingFunction<uint_t>;
using ReadingFunctionFloat  = ReadingFunction<float_t>;
using ReadingFunctionDouble = ReadingFunction<double_t>;

using ReadingFunctionListChar   = ReadingFunctionList<char_t>;
using ReadingFunctionListUchar  = ReadingFunctionList<uchar_t>;
using ReadingFunctionListShort  = ReadingFunctionList<short_t>;
using ReadingFunctionListUshort = ReadingFunctionList<ushort_t>;
using ReadingFunctionListInt    = ReadingFunctionList<int_t>;
using ReadingFunctionListUint   = ReadingFunctionList<uint_t>;
using ReadingFunctionListFloat  = ReadingFunctionList<float_t>;
using ReadingFunctionListDouble = ReadingFunctionList<double_t>;

class Reader
{
public:
    Reader();

    ReadingFunctionChar&   read_char_f();
    ReadingFunctionUchar&  read_uchar_f();
    ReadingFunctionShort&  read_short_f();
    ReadingFunctionUshort& read_ushort_f();
    ReadingFunctionInt&    read_int_f();
    ReadingFunctionUint&   read_uint_f();
    ReadingFunctionFloat&  read_float_f();
    ReadingFunctionDouble& read_double_f();

    ReadingFunctionListChar&   read_char_list_f();
    ReadingFunctionListUchar&  read_uchar_list_f();
    ReadingFunctionListShort&  read_short_list_f();
    ReadingFunctionListUshort& read_ushort_list_f();
    ReadingFunctionListInt&    read_int_list_f();
    ReadingFunctionListUint&   read_uint_list_f();
    ReadingFunctionListFloat&  read_float_list_f();
    ReadingFunctionListDouble& read_double_list_f();

    void read(int i, char_t   value) const;
    void read(int i, uchar_t  value) const;
    void read(int i, short_t  value) const;
    void read(int i, ushort_t value) const;
    void read(int i, int_t    value) const;
    void read(int i, uint_t   value) const;
    void read(int i, float_t  value) const;
    void read(int i, double_t value) const;

    void read(int i, const std::vector<char_t>   values) const;
    void read(int i, const std::vector<uchar_t>  values) const;
    void read(int i, const std::vector<short_t>  values) const;
    void read(int i, const std::vector<ushort_t> values) const;
    void read(int i, const std::vector<int_t>    values) const;
    void read(int i, const std::vector<uint_t>   values) const;
    void read(int i, const std::vector<float_t>  values) const;
    void read(int i, const std::vector<double_t> values) const;

protected:
    ReadingFunctionChar   m_read_char_f;
    ReadingFunctionUchar  m_read_uchar_f;
    ReadingFunctionShort  m_read_short_f;
    ReadingFunctionUshort m_read_ushort_f;
    ReadingFunctionInt    m_read_int_f;
    ReadingFunctionUint   m_read_uint_f;
    ReadingFunctionFloat  m_read_float_f;
    ReadingFunctionDouble m_read_double_f;

    ReadingFunctionListChar   m_read_char_list_f;
    ReadingFunctionListUchar  m_read_uchar_list_f;
    ReadingFunctionListShort  m_read_short_list_f;
    ReadingFunctionListUshort m_read_ushort_list_f;
    ReadingFunctionListInt    m_read_int_list_f;
    ReadingFunctionListUint   m_read_uint_list_f;
    ReadingFunctionListFloat  m_read_float_list_f;
    ReadingFunctionListDouble m_read_double_list_f;
};

// Reading Info ----------------------------------------------------------------

struct ReadingProperty
{
    bool         is_list;
    std::string  name;
    Type         type;
    Type         size_type;

    Reader       reader;
};

struct ReadingElement
{
    std::string name;
    int count;
    std::vector<ReadingProperty> properties;
};

// PLYReader -------------------------------------------------------------------

//!
//! \brief The PLYReader class is the main class for reading ply files.
//!
class PLYReader
{
public:
    PLYReader();

    // Reading -----------------------------------------------------------------
public:
    bool read_header(std::istream& is);
    void read_body(std::istream& is);

    // Internal reading --------------------------------------------------------
protected:
    void read_body_ascii(std::istream& is);
    void read_body_binary(std::istream& is);

    // Reading Info Accessors --------------------------------------------------
public:
    bool ascii() const;
    bool binary() const;
    bool binary_little_endian() const;
    bool binary_big_endian() const;
    int  version() const;

    bool has_element(const std::string& element_name) const;
    bool has_property(const std::string& element_name, const std::string& property_name) const;

    ReadingElement& element(const std::string& element_name);
    ReadingProperty& property(const std::string& element_name, const std::string& property_name);

    const std::vector<std::string>& comments() const;

    const std::vector<ReadingElement>& elements() const;
          std::vector<ReadingElement>& elements();

    int element_count(const std::string& element_name) const;

    // Data --------------------------------------------------------------------
protected:
    bool m_ascii;
    bool m_binary_little_endian;
    bool m_binary_big_endian;
    int  m_version;

    std::vector<std::string> m_comments;
    std::vector<ReadingElement> m_elements;
};


// Writer ----------------------------------------------------------------------

template<typename T>
using WritingFunction     = std::function<T(int i)>;
template<typename T>
using SizeFunction        = std::function<T(int i)>;
template<typename T>
using WritingFunctionList = std::function<T(int i, int k)>;

using WritingFunctionChar   = WritingFunction<char_t>;
using WritingFunctionUchar  = WritingFunction<uchar_t>;
using WritingFunctionShort  = WritingFunction<short_t>;
using WritingFunctionUshort = WritingFunction<ushort_t>;
using WritingFunctionInt    = WritingFunction<int_t>;
using WritingFunctionUint   = WritingFunction<uint_t>;
using WritingFunctionFloat  = WritingFunction<float_t>;
using WritingFunctionDouble = WritingFunction<double_t>;

using SizeFunctionChar   = SizeFunction<char_t>;
using SizeFunctionUchar  = SizeFunction<uchar_t>;
using SizeFunctionShort  = SizeFunction<short_t>;
using SizeFunctionUshort = SizeFunction<ushort_t>;
using SizeFunctionInt    = SizeFunction<int_t>;
using SizeFunctionUint   = SizeFunction<uint_t>;
using SizeFunctionFloat  = SizeFunction<float_t>;
using SizeFunctionDouble = SizeFunction<double_t>;

using WritingFunctionListChar   = WritingFunctionList<char_t>;
using WritingFunctionListUchar  = WritingFunctionList<uchar_t>;
using WritingFunctionListShort  = WritingFunctionList<short_t>;
using WritingFunctionListUshort = WritingFunctionList<ushort_t>;
using WritingFunctionListInt    = WritingFunctionList<int_t>;
using WritingFunctionListUint   = WritingFunctionList<uint_t>;
using WritingFunctionListFloat  = WritingFunctionList<float_t>;
using WritingFunctionListDouble = WritingFunctionList<double_t>;

class Writer
{
public:
    Writer();

    WritingFunctionChar&   write_char_f();
    WritingFunctionUchar&  write_uchar_f();
    WritingFunctionShort&  write_short_f();
    WritingFunctionUshort& write_ushort_f();
    WritingFunctionInt&    write_int_f();
    WritingFunctionUint&   write_uint_f();
    WritingFunctionFloat&  write_float_f();
    WritingFunctionDouble& write_double_f();

    SizeFunctionChar&   size_char_f();
    SizeFunctionUchar&  size_uchar_f();
    SizeFunctionShort&  size_short_f();
    SizeFunctionUshort& size_ushort_f();
    SizeFunctionInt&    size_int_f();
    SizeFunctionUint&   size_uint_f();
    SizeFunctionFloat&  size_float_f();
    SizeFunctionDouble& size_double_f();

    WritingFunctionListChar&   write_char_list_f();
    WritingFunctionListUchar&  write_uchar_list_f();
    WritingFunctionListShort&  write_short_list_f();
    WritingFunctionListUshort& write_ushort_list_f();
    WritingFunctionListInt&    write_int_list_f();
    WritingFunctionListUint&   write_uint_list_f();
    WritingFunctionListFloat&  write_float_list_f();
    WritingFunctionListDouble& write_double_list_f();

    char_t   write_char  (int i) const;
    uchar_t  write_uchar (int i) const;
    short_t  write_short (int i) const;
    ushort_t write_ushort(int i) const;
    int_t    write_int   (int i) const;
    uint_t   write_uint  (int i) const;
    float_t  write_float (int i) const;
    double_t write_double(int i) const;

    char_t   size_char  (int i) const;
    uchar_t  size_uchar (int i) const;
    short_t  size_short (int i) const;
    ushort_t size_ushort(int i) const;
    int_t    size_int   (int i) const;
    uint_t   size_uint  (int i) const;
    float_t  size_float (int i) const;
    double_t size_double(int i) const;

    char_t   write_char  (int i, int k) const;
    uchar_t  write_uchar (int i, int k) const;
    short_t  write_short (int i, int k) const;
    ushort_t write_ushort(int i, int k) const;
    int_t    write_int   (int i, int k) const;
    uint_t   write_uint  (int i, int k) const;
    float_t  write_float (int i, int k) const;
    double_t write_double(int i, int k) const;

protected:
    WritingFunctionChar   m_write_char_f;
    WritingFunctionUchar  m_write_uchar_f;
    WritingFunctionShort  m_write_short_f;
    WritingFunctionUshort m_write_ushort_f;
    WritingFunctionInt    m_write_int_f;
    WritingFunctionUint   m_write_uint_f;
    WritingFunctionFloat  m_write_float_f;
    WritingFunctionDouble m_write_double_f;

    SizeFunctionChar   m_size_char_f;
    SizeFunctionUchar  m_size_uchar_f;
    SizeFunctionShort  m_size_short_f;
    SizeFunctionUshort m_size_ushort_f;
    SizeFunctionInt    m_size_int_f;
    SizeFunctionUint   m_size_uint_f;
    SizeFunctionFloat  m_size_float_f;
    SizeFunctionDouble m_size_double_f;

    WritingFunctionListChar   m_write_char_list_f;
    WritingFunctionListUchar  m_write_uchar_list_f;
    WritingFunctionListShort  m_write_short_list_f;
    WritingFunctionListUshort m_write_ushort_list_f;
    WritingFunctionListInt    m_write_int_list_f;
    WritingFunctionListUint   m_write_uint_list_f;
    WritingFunctionListFloat  m_write_float_list_f;
    WritingFunctionListDouble m_write_double_list_f;
};

// Writing Info ----------------------------------------------------------------

struct WritingProperty
{
    std::string  name;
    Type         type;
    bool         is_list;
    Type         size_type;

    Writer       writer;
};

struct WritingElement
{
    std::string name;
    int count;
    std::vector<WritingProperty> properties;
};

// PLYWriter -------------------------------------------------------------------

//!
//! \brief The PLYWriter class is the main class for writing ply files.
//!
class PLYWriter
{
public:
    PLYWriter();

    // Writing -----------------------------------------------------------------
public:
    void write(std::ostream& os) const;

    // Internal writing --------------------------------------------------------
protected:
    void write_header(std::ostream& os) const;
    void write_body(std::ostream& os) const;

    void write_body_ascii(std::ostream& os) const;
    void write_body_binary(std::ostream& os) const;

    // Writing Info Modifiers --------------------------------------------------
public:
    void set_ascii();
    void set_binary();
    void set_binary_little_endian();
    void set_binary_big_endian();
    void set_version(int version);

    void add_comment(const std::string& comment);

    WritingElement& add_element(const std::string& element_name, int element_count);
    WritingProperty& add_property(const std::string& element_name, const std::string& property_name, Type type);
    WritingProperty& add_property(const std::string& element_name, const std::string& property_name, Type type, Type size_type);

    // Data --------------------------------------------------------------------
protected:
    bool m_ascii;
    bool m_binary_little_endian;
    bool m_binary_big_endian;
    int  m_version;

    std::vector<std::string> m_comments;
    std::vector<WritingElement> m_elements;
};

} // namespace plyio
