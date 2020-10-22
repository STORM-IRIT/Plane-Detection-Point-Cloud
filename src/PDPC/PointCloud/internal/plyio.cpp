#include "plyio.h"

#include <sstream>
#include <algorithm>

namespace plyio {

// String ----------------------------------------------------------------------

//!
//! \brief to_tokens extracts tokens delimited by space in a string
//!
std::vector<std::string> to_tokens(const std::string& str)
{
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    while(std::getline(ss, token, ' '))
    {
        if(!token.empty())
        {
            tokens.emplace_back(std::move(token));
        }
    }
    return tokens;
}

// Types -----------------------------------------------------------------------

Type to_type(const std::string& str)
{
    if(str == "char"  ) return type_char;
    if(str == "uchar" ) return type_uchar;
    if(str == "short" ) return type_short;
    if(str == "ushort") return type_ushort;
    if(str == "int"   ) return type_int;
    if(str == "uint"  ) return type_uint;
    if(str == "float" ) return type_float;
    if(str == "double") return type_double;
    else                return type_unkown;
}

std::string to_string(Type type)
{
    switch(type)
    {
    case type_char:   return "char";
    case type_uchar:  return "uchar";
    case type_short:  return "short";
    case type_ushort: return "ushort";
    case type_int:    return "int";
    case type_uint:   return "uint";
    case type_float:  return "float";
    case type_double: return "double";
    default:          return "unkown";
    }
}

template<> Type type<char_t  >() {return type_char  ;}
template<> Type type<uchar_t >() {return type_uchar ;}
template<> Type type<short_t >() {return type_short ;}
template<> Type type<ushort_t>() {return type_ushort;}
template<> Type type<int_t   >() {return type_int   ;}
template<> Type type<uint_t  >() {return type_uint  ;}
template<> Type type<float_t >() {return type_float ;}
template<> Type type<double_t>() {return type_double;}

// Reader ----------------------------------------------------------------------

// all the functions are initialized to a function that does nothing
// in case the user is not interesting in some properties
Reader::Reader() :
    m_read_char_f([](int,char_t){}),
    m_read_uchar_f([](int,uchar_t){}),
    m_read_short_f([](int,short_t){}),
    m_read_ushort_f([](int,ushort_t){}),
    m_read_int_f([](int,int_t){}),
    m_read_uint_f([](int,uint_t){}),
    m_read_float_f([](int,float_t){}),
    m_read_double_f([](int,double_t){}),
    m_read_char_list_f([](int,const std::vector<char_t>&){}),
    m_read_uchar_list_f([](int,const std::vector<uchar_t>&){}),
    m_read_short_list_f([](int,const std::vector<short_t>&){}),
    m_read_ushort_list_f([](int,const std::vector<ushort_t>&){}),
    m_read_int_list_f([](int,const std::vector<int_t>&){}),
    m_read_uint_list_f([](int,const std::vector<uint_t>&){}),
    m_read_float_list_f([](int,const std::vector<float_t>&){}),
    m_read_double_list_f([](int,const std::vector<double_t>&){})
{
}

ReadingFunctionChar&   Reader::read_char_f()  {return m_read_char_f;  }
ReadingFunctionUchar&  Reader::read_uchar_f() {return m_read_uchar_f; }
ReadingFunctionShort&  Reader::read_short_f() {return m_read_short_f; }
ReadingFunctionUshort& Reader::read_ushort_f(){return m_read_ushort_f;}
ReadingFunctionInt&    Reader::read_int_f()   {return m_read_int_f;   }
ReadingFunctionUint&   Reader::read_uint_f()  {return m_read_uint_f;  }
ReadingFunctionFloat&  Reader::read_float_f() {return m_read_float_f; }
ReadingFunctionDouble& Reader::read_double_f(){return m_read_double_f;}

ReadingFunctionListChar&   Reader::read_char_list_f()  {return m_read_char_list_f;  }
ReadingFunctionListUchar&  Reader::read_uchar_list_f() {return m_read_uchar_list_f; }
ReadingFunctionListShort&  Reader::read_short_list_f() {return m_read_short_list_f; }
ReadingFunctionListUshort& Reader::read_ushort_list_f(){return m_read_ushort_list_f;}
ReadingFunctionListInt&    Reader::read_int_list_f()   {return m_read_int_list_f;   }
ReadingFunctionListUint&   Reader::read_uint_list_f()  {return m_read_uint_list_f;  }
ReadingFunctionListFloat&  Reader::read_float_list_f() {return m_read_float_list_f; }
ReadingFunctionListDouble& Reader::read_double_list_f(){return m_read_double_list_f;}

void Reader::read(int i, char_t   value) const {m_read_char_f(i,value);  }
void Reader::read(int i, uchar_t  value) const {m_read_uchar_f(i,value); }
void Reader::read(int i, short_t  value) const {m_read_short_f(i,value); }
void Reader::read(int i, ushort_t value) const {m_read_ushort_f(i,value);}
void Reader::read(int i, int_t    value) const {m_read_int_f(i,value);   }
void Reader::read(int i, uint_t   value) const {m_read_uint_f(i,value);  }
void Reader::read(int i, float_t  value) const {m_read_float_f(i,value); }
void Reader::read(int i, double_t value) const {m_read_double_f(i,value);}

void Reader::read(int i, const std::vector<char_t>   values) const {m_read_char_list_f(i,values);  }
void Reader::read(int i, const std::vector<uchar_t>  values) const {m_read_uchar_list_f(i,values); }
void Reader::read(int i, const std::vector<short_t>  values) const {m_read_short_list_f(i,values); }
void Reader::read(int i, const std::vector<ushort_t> values) const {m_read_ushort_list_f(i,values);}
void Reader::read(int i, const std::vector<int_t>    values) const {m_read_int_list_f(i,values);   }
void Reader::read(int i, const std::vector<uint_t>   values) const {m_read_uint_list_f(i,values);  }
void Reader::read(int i, const std::vector<float_t>  values) const {m_read_float_list_f(i,values); }
void Reader::read(int i, const std::vector<double_t> values) const {m_read_double_list_f(i,values);}

// PLYReader -------------------------------------------------------------------

PLYReader::PLYReader() :
    m_ascii(false),
    m_binary_little_endian(false),
    m_binary_big_endian(false),
    m_version(0),
    m_comments(0),
    m_elements(0)
{
}

// Reading ---------------------------------------------------------------------

bool PLYReader::read_header(std::istream& is)
{
    // reset
    m_ascii = false;
    m_binary_little_endian = true;
    m_binary_big_endian = false;
    m_version = 0;
    m_comments.clear();
    m_elements.clear();

    std::string line;

    std::getline(is, line);
    if(line != "ply") return false;

    bool end_header_found = false;
    while(std::getline(is, line))
    {
        std::vector<std::string> tokens = to_tokens(line);

        if(tokens.empty()) continue; // skip empty lines

        if(line == "end_header")
        {
            end_header_found = true;
            break;
        }
        if(tokens.front() == "format")
        {
            if(tokens.size() != 3) return false;
            m_ascii                 = tokens[1] == "ascii";
            m_binary_little_endian  = tokens[1] == "binary_little_endian";
            m_binary_big_endian     = tokens[1] == "binary_big_endian";
            m_version               = std::stoi(tokens[2]); //TODO version is not one single integer
        }
        if(tokens.front() == "comment")
        {
            m_comments.emplace_back(line.substr(8));
        }
        if(tokens.front() == "element")
        {
            if(tokens.size() != 3) return false;
            std::string name = tokens[1];
            int count = std::stoi(tokens[2]);
            m_elements.push_back({name,count,{/*empty properties*/}});
        }
        if(tokens.front() == "property")
        {
            if(m_elements.empty() || tokens.size() < 2) return false;

            if(tokens[1] == "list")
            {
                if(tokens.size() != 5) return false;
                bool is_list = true;
                Type type = to_type(tokens[3]);
                std::string name = tokens[4];
                Type size_type = to_type(tokens[2]);
                m_elements.back().properties.push_back({is_list,name,type,size_type,{/*reader*/}});
            }
            else if(tokens.size() == 3)
            {
                bool is_list = false;
                Type type = to_type(tokens[1]);
                std::string name = tokens[2];
                Type size_type = type_unkown;
                m_elements.back().properties.push_back({is_list,name,type,size_type,{/*reader*/}});
            }
            else return false;
        }
    }
    return end_header_found;
}

void PLYReader::read_body(std::istream& is)
{
    if(m_ascii)
    {
        this->read_body_ascii(is);
    }
    else if(m_binary_big_endian || m_binary_little_endian)
    {
        this->read_body_binary(is);
    }
}

// Internal reading ------------------------------------------------------------

void PLYReader::read_body_ascii(std::istream& is)
{
    std::vector<char_t>   vec0;
    std::vector<uchar_t>  vec1;
    std::vector<short_t>  vec2;
    std::vector<ushort_t> vec3;
    std::vector<int_t>    vec4;
    std::vector<uint_t>   vec5;
    std::vector<float_t>  vec6;
    std::vector<double_t> vec7;

    char_t   val0;
    uchar_t  val1;
    short_t  val2;
    ushort_t val3;
    int_t    val4;
    uint_t   val5;
    float_t  val6;
    double_t val7;

    short_t  tmp0;
    ushort_t tmp1;

    for(uint idx_element = 0; idx_element < m_elements.size(); ++idx_element)
    {
        const ReadingElement& element = m_elements[idx_element];
        for(int i = 0; i < element.count; ++i)
        {
            for(uint idx_property = 0; idx_property < element.properties.size(); ++idx_property)
            {
                const ReadingProperty& property = element.properties[idx_property];
                if(property.is_list)
                {
                    size_t size = 0;
                    switch (property.size_type)
                    {
                    case type_char:    is >> tmp0; val0 = tmp0; size = val0; break;
                    case type_uchar:   is >> tmp1; val1 = tmp1; size = val1; break;
                    case type_short:   is >> val2;              size = val2; break;
                    case type_ushort:  is >> val3;              size = val3; break;
                    case type_int:     is >> val4;              size = val4; break;
                    case type_uint:    is >> val5;              size = val5; break;
                    case type_float:   is >> val6;              size = val6; break;
                    case type_double:  is >> val7;              size = val7; break;
                    case type_unkown: break;
                    default:          break;
                    }

                    switch (property.type)
                    {
                    case type_char:   vec0.resize(size); for(size_t k=0; k<size; ++k) {is >> tmp0; vec0[k] = tmp0;} property.reader.read(i, vec0); break;
                    case type_uchar:  vec1.resize(size); for(size_t k=0; k<size; ++k) {is >> tmp1; vec1[k] = tmp1;} property.reader.read(i, vec1); break;
                    case type_short:  vec2.resize(size); for(size_t k=0; k<size; ++k) {is >> vec2[k];             } property.reader.read(i, vec2); break;
                    case type_ushort: vec3.resize(size); for(size_t k=0; k<size; ++k) {is >> vec3[k];             } property.reader.read(i, vec3); break;
                    case type_int:    vec4.resize(size); for(size_t k=0; k<size; ++k) {is >> vec4[k];             } property.reader.read(i, vec4); break;
                    case type_uint:   vec5.resize(size); for(size_t k=0; k<size; ++k) {is >> vec5[k];             } property.reader.read(i, vec5); break;
                    case type_float:  vec6.resize(size); for(size_t k=0; k<size; ++k) {is >> vec6[k];             } property.reader.read(i, vec6); break;
                    case type_double: vec7.resize(size); for(size_t k=0; k<size; ++k) {is >> vec7[k];             } property.reader.read(i, vec7); break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
                else
                {
                    switch (property.type)
                    {
                    case type_char:   is >> tmp0; val0 = tmp0; property.reader.read(i, val0); break;
                    case type_uchar:  is >> tmp1; val1 = tmp1; property.reader.read(i, val1); break;
                    case type_short:  is >> val2;              property.reader.read(i, val2); break;
                    case type_ushort: is >> val3;              property.reader.read(i, val3); break;
                    case type_int:    is >> val4;              property.reader.read(i, val4); break;
                    case type_uint:   is >> val5;              property.reader.read(i, val5); break;
                    case type_float:  is >> val6;              property.reader.read(i, val6); break;
                    case type_double: is >> val7;              property.reader.read(i, val7); break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
            }
        }
    }
}

void PLYReader::read_body_binary(std::istream& is)
{
    std::vector<char_t>   vec0;
    std::vector<uchar_t>  vec1;
    std::vector<short_t>  vec2;
    std::vector<ushort_t> vec3;
    std::vector<int_t>    vec4;
    std::vector<uint_t>   vec5;
    std::vector<float_t>  vec6;
    std::vector<double_t> vec7;

    char_t   val0;
    uchar_t  val1;
    short_t  val2;
    ushort_t val3;
    int_t    val4;
    uint_t   val5;
    float_t  val6;
    double_t val7;

//    short_t  tmp0;
//    ushort_t tmp1;

    for(uint idx_element = 0; idx_element < m_elements.size(); ++idx_element)
    {
        const ReadingElement& element = m_elements[idx_element];
        for(int i = 0; i < element.count; ++i)
        {
            for(uint idx_property = 0; idx_property < element.properties.size(); ++idx_property)
            {
                const ReadingProperty& property = element.properties[idx_property];
                if(property.is_list)
                {
                    size_t size = 0;
                    switch (property.size_type)
                    {
                    case type_char:       is.read(reinterpret_cast<char*>(&val0), 1); size = val0; vec0.resize(size); break;
                    case type_uchar:      is.read(reinterpret_cast<char*>(&val1), 1); size = val1; vec1.resize(size); break;
                    case type_short:      is.read(reinterpret_cast<char*>(&val2), 2); size = val2; vec2.resize(size); break;
                    case type_ushort:     is.read(reinterpret_cast<char*>(&val3), 2); size = val3; vec3.resize(size); break;
                    case type_int:        is.read(reinterpret_cast<char*>(&val4), 4); size = val4; vec4.resize(size); break;
                    case type_uint:       is.read(reinterpret_cast<char*>(&val5), 4); size = val5; vec5.resize(size); break;
                    case type_float:      is.read(reinterpret_cast<char*>(&val6), 4); size = val6; vec6.resize(size); break;
                    case type_double:     is.read(reinterpret_cast<char*>(&val7), 8); size = val7; vec7.resize(size); break;
                    case type_unkown:     break;
                    default:              break;
                    }

                    switch (property.type)
                    {
                    case type_char:       vec0.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec0[k]), 1);} property.reader.read(i, vec0); break;
                    case type_uchar:      vec1.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec1[k]), 1);} property.reader.read(i, vec1); break;
                    case type_short:      vec2.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec2[k]), 2);} property.reader.read(i, vec2); break;
                    case type_ushort:     vec3.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec3[k]), 2);} property.reader.read(i, vec3); break;
                    case type_int:        vec4.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec4[k]), 4);} property.reader.read(i, vec4); break;
                    case type_uint:       vec5.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec5[k]), 4);} property.reader.read(i, vec5); break;
                    case type_float:      vec6.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec6[k]), 4);} property.reader.read(i, vec6); break;
                    case type_double:     vec7.resize(size); for(size_t k=0; k<size; ++k) {is.read(reinterpret_cast<char*>(&vec7[k]), 8);} property.reader.read(i, vec7); break;
                    case type_unkown:     break;
                    default:              break;
                    }
                }
                else
                {
                    switch (property.type)
                    {
                    case type_char:       is.read(reinterpret_cast<char*>(&val0), 1); property.reader.read(i,val0); break;
                    case type_uchar:      is.read(reinterpret_cast<char*>(&val1), 1); property.reader.read(i,val1); break;
                    case type_short:      is.read(reinterpret_cast<char*>(&val2), 2); property.reader.read(i,val2); break;
                    case type_ushort:     is.read(reinterpret_cast<char*>(&val3), 2); property.reader.read(i,val3); break;
                    case type_int:        is.read(reinterpret_cast<char*>(&val4), 4); property.reader.read(i,val4); break;
                    case type_uint:       is.read(reinterpret_cast<char*>(&val5), 4); property.reader.read(i,val5); break;
                    case type_float:      is.read(reinterpret_cast<char*>(&val6), 4); property.reader.read(i,val6); break;
                    case type_double:     is.read(reinterpret_cast<char*>(&val7), 8); property.reader.read(i,val7); break;
                    case type_unkown:     break;
                    default:                            break;
                    }
                }
            }
        }
    }
}

// Reading Info Accessors ------------------------------------------------------

bool PLYReader::ascii() const
{
    return m_ascii;
}

bool PLYReader::binary() const
{
    return m_binary_big_endian || m_binary_little_endian;
}

bool PLYReader::binary_little_endian() const
{
    return m_binary_little_endian;
}

bool PLYReader::binary_big_endian() const
{
    return m_binary_big_endian;
}

int PLYReader::version() const
{
    return m_version;
}

bool PLYReader::has_element(const std::string& element_name) const
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const ReadingElement& e)
    {
        return e.name == element_name;
    });
    return it != m_elements.end();
}

bool PLYReader::has_property(const std::string& element_name, const std::string& property_name) const
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const ReadingElement& e)
    {
        return e.name == element_name;
    });
    if(it == m_elements.end())
    {
        return false;
    }
    const ReadingElement& e = *it;
    auto it2 = std::find_if(e.properties.begin(), e.properties.end(), [&property_name](const ReadingProperty& p)
    {
        return p.name == property_name;
    });
    return it2 != e.properties.end();
}

ReadingElement& PLYReader::element(const std::string& element_name)
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const ReadingElement& e)
    {
        return e.name == element_name;
    });
    return *it;
}

ReadingProperty& PLYReader::property(const std::string& element_name, const std::string& property_name)
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const ReadingElement& e)
    {
        return e.name == element_name;
    });
    ReadingElement& e = *it;
    auto it2 = std::find_if(e.properties.begin(), e.properties.end(), [&property_name](const ReadingProperty& p)
    {
        return p.name == property_name;
    });
    return *it2;
}

const std::vector<std::string>& PLYReader::comments() const
{
    return m_comments;
}

const std::vector<ReadingElement>& PLYReader::elements() const
{
    return m_elements;
}

std::vector<ReadingElement>& PLYReader::elements()
{
    return m_elements;
}

int PLYReader::element_count(const std::string& element_name) const
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
                           [&element_name](const ReadingElement& e) -> bool
    {
        return e.name == element_name;
    });
    return it == m_elements.end() ? 0 : it->count;
}

// Writer ----------------------------------------------------------------------

Writer::Writer() :
    m_write_char_f(),
    m_write_uchar_f(),
    m_write_short_f(),
    m_write_ushort_f(),
    m_write_int_f(),
    m_write_uint_f(),
    m_write_float_f(),
    m_write_double_f(),
    m_size_char_f(),
    m_size_uchar_f(),
    m_size_short_f(),
    m_size_ushort_f(),
    m_size_int_f(),
    m_size_uint_f(),
    m_size_float_f(),
    m_size_double_f(),
    m_write_char_list_f(),
    m_write_uchar_list_f(),
    m_write_short_list_f(),
    m_write_ushort_list_f(),
    m_write_int_list_f(),
    m_write_uint_list_f(),
    m_write_float_list_f(),
    m_write_double_list_f()
{
}

WritingFunctionChar&   Writer::write_char_f()   {return m_write_char_f;  }
WritingFunctionUchar&  Writer::write_uchar_f()  {return m_write_uchar_f; }
WritingFunctionShort&  Writer::write_short_f()  {return m_write_short_f; }
WritingFunctionUshort& Writer::write_ushort_f() {return m_write_ushort_f;}
WritingFunctionInt&    Writer::write_int_f()    {return m_write_int_f;   }
WritingFunctionUint&   Writer::write_uint_f()   {return m_write_uint_f;  }
WritingFunctionFloat&  Writer::write_float_f()  {return m_write_float_f; }
WritingFunctionDouble& Writer::write_double_f() {return m_write_double_f;}

SizeFunctionChar&   Writer::size_char_f()   {return m_size_char_f;  }
SizeFunctionUchar&  Writer::size_uchar_f()  {return m_size_uchar_f; }
SizeFunctionShort&  Writer::size_short_f()  {return m_size_short_f; }
SizeFunctionUshort& Writer::size_ushort_f() {return m_size_ushort_f;}
SizeFunctionInt&    Writer::size_int_f()    {return m_size_int_f;   }
SizeFunctionUint&   Writer::size_uint_f()   {return m_size_uint_f;  }
SizeFunctionFloat&  Writer::size_float_f()  {return m_size_float_f; }
SizeFunctionDouble& Writer::size_double_f() {return m_size_double_f;}

WritingFunctionListChar&   Writer::write_char_list_f()   {return m_write_char_list_f;  }
WritingFunctionListUchar&  Writer::write_uchar_list_f()  {return m_write_uchar_list_f; }
WritingFunctionListShort&  Writer::write_short_list_f()  {return m_write_short_list_f; }
WritingFunctionListUshort& Writer::write_ushort_list_f() {return m_write_ushort_list_f;}
WritingFunctionListInt&    Writer::write_int_list_f()    {return m_write_int_list_f;   }
WritingFunctionListUint&   Writer::write_uint_list_f()   {return m_write_uint_list_f;  }
WritingFunctionListFloat&  Writer::write_float_list_f()  {return m_write_float_list_f; }
WritingFunctionListDouble& Writer::write_double_list_f() {return m_write_double_list_f;}

char_t   Writer::write_char  (int i) const {return m_write_char_f(i);  }
uchar_t  Writer::write_uchar (int i) const {return m_write_uchar_f(i); }
short_t  Writer::write_short (int i) const {return m_write_short_f(i); }
ushort_t Writer::write_ushort(int i) const {return m_write_ushort_f(i);}
int_t    Writer::write_int   (int i) const {return m_write_int_f(i);   }
uint_t   Writer::write_uint  (int i) const {return m_write_uint_f(i);  }
float_t  Writer::write_float (int i) const {return m_write_float_f(i); }
double_t Writer::write_double(int i) const {return m_write_double_f(i);}

char_t   Writer::size_char  (int i) const {return m_size_char_f(i);  }
uchar_t  Writer::size_uchar (int i) const {return m_size_uchar_f(i); }
short_t  Writer::size_short (int i) const {return m_size_short_f(i); }
ushort_t Writer::size_ushort(int i) const {return m_size_ushort_f(i);}
int_t    Writer::size_int   (int i) const {return m_size_int_f(i);   }
uint_t   Writer::size_uint  (int i) const {return m_size_uint_f(i);  }
float_t  Writer::size_float (int i) const {return m_size_float_f(i); }
double_t Writer::size_double(int i) const {return m_size_double_f(i);}

char_t   Writer::write_char  (int i, int k) const {return m_write_char_list_f(i,k);  }
uchar_t  Writer::write_uchar (int i, int k) const {return m_write_uchar_list_f(i,k); }
short_t  Writer::write_short (int i, int k) const {return m_write_short_list_f(i,k); }
ushort_t Writer::write_ushort(int i, int k) const {return m_write_ushort_list_f(i,k);}
int_t    Writer::write_int   (int i, int k) const {return m_write_int_list_f(i,k);   }
uint_t   Writer::write_uint  (int i, int k) const {return m_write_uint_list_f(i,k);  }
float_t  Writer::write_float (int i, int k) const {return m_write_float_list_f(i,k); }
double_t Writer::write_double(int i, int k) const {return m_write_double_list_f(i,k);}

// PLYWriter -------------------------------------------------------------------

PLYWriter::PLYWriter() :
    m_ascii(false),
    m_binary_little_endian(true),
    m_binary_big_endian(false),
    m_version(1),
    m_comments(0),
    m_elements(0)
{
}

// Writing ---------------------------------------------------------------------

void PLYWriter::write(std::ostream& os) const
{
    this->write_header(os);
    this->write_body(os);
}

// Internal writing ------------------------------------------------------------

void PLYWriter::write_header(std::ostream& os) const
{
    os << "ply\n";
    os << "format ";

    if(m_ascii)
    {
        os << "ascii";
    }
    else if(m_binary_little_endian)
    {
        os << "binary_little_endian";
    }
    else if(m_binary_big_endian)
    {
        os << "binary_big_endian";
    }

    os << " " << m_version << ".0\n";   //TODO make this format clear: 1.0?

    for(const std::string& comment : m_comments)
    {
        os << "comment " << comment << "\n";
    }

    for(const WritingElement& element : m_elements)
    {
        os << "element " << element.name << " " << element.count << "\n";
        for(const WritingProperty& property : element.properties)
        {
            if(property.is_list)
            {
                os << "property list " << to_string(property.size_type) << " " << to_string(property.type) << " " << property.name << "\n";
            }
            else
            {
                os << "property " << to_string(property.type) << " " << property.name << "\n";
            }
        }
    }

    os << "end_header\n";
}

void PLYWriter::write_body(std::ostream& os) const
{
    if(m_ascii)
    {
        this->write_body_ascii(os);
    }
    else if(m_binary_big_endian || m_binary_little_endian)
    {
        this->write_body_binary(os);
    }
}

void PLYWriter::write_body_ascii(std::ostream& os) const
{
    char_t   size0;
    uchar_t  size1;
    short_t  size2;
    ushort_t size3;
    int_t    size4;
    uint_t   size5;
    float_t  size6;
    double_t size7;

    for(uint idx_element = 0; idx_element < m_elements.size(); ++idx_element)
    {
        const WritingElement& element = m_elements[idx_element];
        for(int i = 0; i < element.count; ++i)
        {
            for(uint idx_property = 0; idx_property < element.properties.size(); ++idx_property)
            {
                const WritingProperty& property = element.properties[idx_property];
                if(property.is_list)
                {
                    size_t size = 0;
                    switch (property.size_type)
                    {
                    case type_char:    size0 = property.writer.size_char(i);   size = size0; os <<  short_t(size0); break;
                    case type_uchar:   size1 = property.writer.size_uchar(i);  size = size1; os << ushort_t(size1); break;
                    case type_short:   size2 = property.writer.size_short(i);  size = size2; os << size2; break;
                    case type_ushort:  size3 = property.writer.size_ushort(i); size = size3; os << size3; break;
                    case type_int:     size4 = property.writer.size_int(i);    size = size4; os << size4; break;
                    case type_uint:    size5 = property.writer.size_uint(i);   size = size5; os << size5; break;
                    case type_float:   size6 = property.writer.size_float(i);  size = size6; os << size6; break;
                    case type_double:  size7 = property.writer.size_double(i); size = size7; os << size7; break;
                    case type_unkown:  break;
                    default:           break;
                    }
                    os << " ";

                    switch (property.type)
                    {
                    case type_char:   for(size_t k=0; k<size; ++k) {os <<  short_t(property.writer.write_char (i,k)) << " ";} break;
                    case type_uchar:  for(size_t k=0; k<size; ++k) {os << ushort_t(property.writer.write_uchar(i,k)) << " ";} break;
                    case type_short:  for(size_t k=0; k<size; ++k) {os << property.writer.write_short (i,k) << " ";} break;
                    case type_ushort: for(size_t k=0; k<size; ++k) {os << property.writer.write_ushort(i,k) << " ";} break;
                    case type_int:    for(size_t k=0; k<size; ++k) {os << property.writer.write_int   (i,k) << " ";} break;
                    case type_uint:   for(size_t k=0; k<size; ++k) {os << property.writer.write_uint  (i,k) << " ";} break;
                    case type_float:  for(size_t k=0; k<size; ++k) {os << property.writer.write_float (i,k) << " ";} break;
                    case type_double: for(size_t k=0; k<size; ++k) {os << property.writer.write_double(i,k) << " ";} break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
                else
                {
                    switch (property.type)
                    {
                    case type_char:   os <<  short_t(property.writer.write_char  (i)); break;
                    case type_uchar:  os << ushort_t(property.writer.write_uchar (i)); break;
                    case type_short:  os << property.writer.write_short (i); break;
                    case type_ushort: os << property.writer.write_ushort(i); break;
                    case type_int:    os << property.writer.write_int   (i); break;
                    case type_uint:   os << property.writer.write_uint  (i); break;
                    case type_float:  os << property.writer.write_float (i); break;
                    case type_double: os << property.writer.write_double(i); break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
                os << " ";
            }
            os << "\n";
        }
    }
}

void PLYWriter::write_body_binary(std::ostream& os) const
{
    char_t   val0;
    uchar_t  val1;
    short_t  val2;
    ushort_t val3;
    int_t    val4;
    uint_t   val5;
    float_t  val6;
    double_t val7;

    for(uint idx_element = 0; idx_element < m_elements.size(); ++idx_element)
    {
        const WritingElement& element = m_elements[idx_element];
        for(int i = 0; i < element.count; ++i)
        {
            for(uint idx_property = 0; idx_property < element.properties.size(); ++idx_property)
            {
                const WritingProperty& property = element.properties[idx_property];
                if(property.is_list)
                {
                    size_t size = 0;
                    switch (property.size_type)
                    {
                    case type_char:    val0 = property.writer.size_char(i);   size = val0; os.write(reinterpret_cast<const char*>(&val0), 1); break;
                    case type_uchar:   val1 = property.writer.size_uchar(i);  size = val1; os.write(reinterpret_cast<const char*>(&val1), 1); break;
                    case type_short:   val2 = property.writer.size_short(i);  size = val2; os.write(reinterpret_cast<const char*>(&val2), 2); break;
                    case type_ushort:  val3 = property.writer.size_ushort(i); size = val3; os.write(reinterpret_cast<const char*>(&val3), 2); break;
                    case type_int:     val4 = property.writer.size_int(i);    size = val4; os.write(reinterpret_cast<const char*>(&val4), 4); break;
                    case type_uint:    val5 = property.writer.size_uint(i);   size = val5; os.write(reinterpret_cast<const char*>(&val5), 4); break;
                    case type_float:   val6 = property.writer.size_float(i);  size = val6; os.write(reinterpret_cast<const char*>(&val6), 4); break;
                    case type_double:  val7 = property.writer.size_double(i); size = val7; os.write(reinterpret_cast<const char*>(&val7), 8); break;
                    case type_unkown:  break;
                    default:           break;
                    }

                    switch (property.type)
                    {
                    case type_char:   for(size_t k=0; k<size; ++k) {val0 = property.writer.write_char  (i,k); os.write(reinterpret_cast<const char*>(&val0), 1);} break;
                    case type_uchar:  for(size_t k=0; k<size; ++k) {val1 = property.writer.write_uchar (i,k); os.write(reinterpret_cast<const char*>(&val1), 1);} break;
                    case type_short:  for(size_t k=0; k<size; ++k) {val2 = property.writer.write_short (i,k); os.write(reinterpret_cast<const char*>(&val2), 2);} break;
                    case type_ushort: for(size_t k=0; k<size; ++k) {val3 = property.writer.write_ushort(i,k); os.write(reinterpret_cast<const char*>(&val3), 2);} break;
                    case type_int:    for(size_t k=0; k<size; ++k) {val4 = property.writer.write_int   (i,k); os.write(reinterpret_cast<const char*>(&val4), 4);} break;
                    case type_uint:   for(size_t k=0; k<size; ++k) {val5 = property.writer.write_uint  (i,k); os.write(reinterpret_cast<const char*>(&val5), 4);} break;
                    case type_float:  for(size_t k=0; k<size; ++k) {val6 = property.writer.write_float (i,k); os.write(reinterpret_cast<const char*>(&val6), 4);} break;
                    case type_double: for(size_t k=0; k<size; ++k) {val7 = property.writer.write_double(i,k); os.write(reinterpret_cast<const char*>(&val7), 8);} break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
                else
                {
                    switch (property.type)
                    {
                    case type_char:   val0 = property.writer.write_char  (i); os.write(reinterpret_cast<const char*>(&val0), 1); break;
                    case type_uchar:  val1 = property.writer.write_uchar (i); os.write(reinterpret_cast<const char*>(&val1), 1); break;
                    case type_short:  val2 = property.writer.write_short (i); os.write(reinterpret_cast<const char*>(&val2), 2); break;
                    case type_ushort: val3 = property.writer.write_ushort(i); os.write(reinterpret_cast<const char*>(&val3), 2); break;
                    case type_int:    val4 = property.writer.write_int   (i); os.write(reinterpret_cast<const char*>(&val4), 4); break;
                    case type_uint:   val5 = property.writer.write_uint  (i); os.write(reinterpret_cast<const char*>(&val5), 4); break;
                    case type_float:  val6 = property.writer.write_float (i); os.write(reinterpret_cast<const char*>(&val6), 4); break;
                    case type_double: val7 = property.writer.write_double(i); os.write(reinterpret_cast<const char*>(&val7), 8); break;
                    case type_unkown: break;
                    default:          break;
                    }
                }
            }
        }
    }
}

// Writing Info Modifiers ------------------------------------------------------

void PLYWriter::set_ascii()
{
    m_ascii                 = true;
    m_binary_little_endian  = false;
    m_binary_big_endian     = false;
}

void PLYWriter::set_binary()
{
    m_ascii                 = false;
    m_binary_little_endian  = true;
    m_binary_big_endian     = false;
}

void PLYWriter::set_binary_little_endian()
{
    m_ascii                 = false;
    m_binary_little_endian  = true;
    m_binary_big_endian     = false;
}

void PLYWriter::set_binary_big_endian()
{
    m_ascii                 = false;
    m_binary_little_endian  = false;
    m_binary_big_endian     = true;
}

void PLYWriter::set_version(int version)
{
    m_version = version;
}

void PLYWriter::add_comment(const std::string& comment)
{
    m_comments.emplace_back(comment);
}

WritingElement& PLYWriter::add_element(const std::string& element_name, int element_count)
{
    m_elements.push_back({element_name, element_count, {/*properties*/}});
    return m_elements.back();
}

WritingProperty& PLYWriter::add_property(const std::string& element_name, const std::string& property_name, Type type)
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const WritingElement& e)
    {
        return e.name == element_name;
    });

    it->properties.push_back({property_name,type,false,{/*...*/},{/*writer*/}});

    return it->properties.back();
}

WritingProperty& PLYWriter::add_property(const std::string& element_name, const std::string& property_name, Type type, Type size_type)
{
    auto it = std::find_if(m_elements.begin(), m_elements.end(), [&element_name](const WritingElement& e)
    {
        return e.name == element_name;
    });

    it->properties.push_back({property_name,type,true,size_type,{/*writer*/}});

    return it->properties.back();
}

} // namespace plyio
