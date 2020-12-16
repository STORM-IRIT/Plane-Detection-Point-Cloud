#pragma once

#include <PDPC/Common/Log.h>
#include <PDPC/Common/Assert.h>

#include <Eigen/Core>

#include <fstream>

namespace pdpc {

class IO
{
    // TypeID ------------------------------------------------------------------
public:
    enum TypeID : int
    {
        // Fundamental types
        ID_BOOL = 0,
        ID_INT,
        ID_UINT,
        ID_FLOAT,
        ID_DOUBLE,
        // Other
        ID_STRING,
        // Eigen vector types
        ID_EIGEN_VECTOR2I,
        ID_EIGEN_VECTOR3I,
        ID_EIGEN_VECTOR4I,
        ID_EIGEN_VECTOR2UI,
        ID_EIGEN_VECTOR3UI,
        ID_EIGEN_VECTOR4UI,
        ID_EIGEN_VECTOR2F,
        ID_EIGEN_VECTOR3F,
        ID_EIGEN_VECTOR4F,
        ID_EIGEN_VECTOR2D,
        ID_EIGEN_VECTOR3D,
        ID_EIGEN_VECTOR4D,
        // Unkown type
        ID_INVALID
    };

    static inline std::string type_id(TypeID id);
    static inline TypeID type_id(const std::string& id);
    template<typename T>
    static inline TypeID type_id();

    // TypeID IO ---------------------------------------------------------------
public:
    static inline TypeID read(std::istream& is);
    static inline void write(TypeID id, std::ostream& os);

    // IO ----------------------------------------------------------------------
public:
    template<typename T> static inline std::istream& read_value(std::istream& is, T& val);
    template<typename T> static inline std::ostream& write_value(std::ostream& os, const T& val);
    static inline std::istream& read_value(std::istream& is, std::string& val);
    static inline std::ostream& write_value(std::ostream& os, const std::string& val);

    template<typename T> static inline std::string to_string(const T& value);
    static inline std::string to_string(const std::string& value);
    static inline std::string to_string(const Eigen::Vector2i& value);
    static inline std::string to_string(const Eigen::Vector3i& value);
    static inline std::string to_string(const Eigen::Vector4i& value);
    static inline std::string to_string(const Eigen::Matrix<uint,2,1>& value);
    static inline std::string to_string(const Eigen::Matrix<uint,3,1>& value);
    static inline std::string to_string(const Eigen::Matrix<uint,4,1>& value);
    static inline std::string to_string(const Eigen::Vector2f& value);
    static inline std::string to_string(const Eigen::Vector3f& value);
    static inline std::string to_string(const Eigen::Vector4f& value);
    static inline std::string to_string(const Eigen::Vector2d& value);
    static inline std::string to_string(const Eigen::Vector3d& value);
    static inline std::string to_string(const Eigen::Vector4d& value);

    template<typename T> static inline void from_string(T& value, const std::string& str);
    static inline void from_string(bool& value, const std::string& str);
    static inline void from_string(int& value, const std::string& str);
    static inline void from_string(uint& value, const std::string& str);
    static inline void from_string(float& value, const std::string& str);
    static inline void from_string(double& value, const std::string& str);
    static inline void from_string(std::string& value, const std::string& str);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// TypeID ----------------------------------------------------------------------

std::string IO::type_id(TypeID id)
{
    switch(id)
    {
    case ID_BOOL:   return "bool";
    case ID_INT:    return "int";
    case ID_UINT:   return "uint";
    case ID_FLOAT:  return "float";
    case ID_DOUBLE: return "double";
    default:        return "INVALID";
    }
}

IO::TypeID IO::type_id(const std::string& id)
{
         if(id=="bool")     return ID_BOOL;
    else if(id=="int")      return ID_INT;
    else if(id=="uint")     return ID_UINT;
    else if(id=="float")    return ID_FLOAT;
    else if(id=="double")   return ID_DOUBLE;
    else                    return ID_INVALID;
}

// Unkown type
template<typename T> IO::TypeID IO::type_id()                   {return ID_INVALID;}
// Fundamental types
template<>    inline IO::TypeID IO::type_id<bool>()             {return ID_BOOL;}
template<>    inline IO::TypeID IO::type_id<int>()              {return ID_INT;}
template<>    inline IO::TypeID IO::type_id<unsigned int>()     {return ID_UINT;}
template<>    inline IO::TypeID IO::type_id<float>()            {return ID_FLOAT;}
template<>    inline IO::TypeID IO::type_id<double>()           {return ID_DOUBLE;}
// Other
template<>    inline IO::TypeID IO::type_id<std::string>()      {return ID_STRING;}
// Eigen vector types
template<>    inline IO::TypeID IO::type_id<Eigen::Vector2i>()  {return ID_EIGEN_VECTOR2I;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector3i>()  {return ID_EIGEN_VECTOR3I;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector4i>()  {return ID_EIGEN_VECTOR4I;}
template<>    inline IO::TypeID IO::type_id<Eigen::Matrix<uint,2,1>>() {return ID_EIGEN_VECTOR2UI;}
template<>    inline IO::TypeID IO::type_id<Eigen::Matrix<uint,3,1>>() {return ID_EIGEN_VECTOR3UI;}
template<>    inline IO::TypeID IO::type_id<Eigen::Matrix<uint,4,1>>() {return ID_EIGEN_VECTOR4UI;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector2f>()  {return ID_EIGEN_VECTOR2F;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector3f>()  {return ID_EIGEN_VECTOR3F;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector4f>()  {return ID_EIGEN_VECTOR4F;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector2d>()  {return ID_EIGEN_VECTOR2D;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector3d>()  {return ID_EIGEN_VECTOR3D;}
template<>    inline IO::TypeID IO::type_id<Eigen::Vector4d>()  {return ID_EIGEN_VECTOR4D;}

// IO --------------------------------------------------------------------------

template<typename T>
std::istream& IO::read_value(std::istream& is, T& val)
{
    return is.read(reinterpret_cast<char*>(&val), sizeof(T));
}

template<typename T>
std::ostream& IO::write_value(std::ostream& os, const T& val)
{
    return os.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

std::istream& IO::read_value(std::istream& is, std::string& val)
{
    int size = -1;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));
    PDPC_DEBUG_ASSERT(size >= 0);
    val.resize(size);
    is.read(&val[0], size * sizeof(char));
    return is;
}

std::ostream& IO::write_value(std::ostream& os, const std::string& val)
{
    int size = val.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(int));
    os.write(&val[0], size * sizeof(char));
    return os;
}

template<typename T>
std::string IO::to_string(const T& value)
{
    return std::to_string(value);
}

std::string IO::to_string(const std::string& value)
{
    return value;
}

std::string IO::to_string(const Eigen::Vector2i&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector3i&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector4i&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Matrix<uint,2,1>&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Matrix<uint,3,1>&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Matrix<uint,4,1>&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector2f&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector3f&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector4f&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector2d&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector3d&) {PDPC_DEBUG_ASSERT(false); return "";}
std::string IO::to_string(const Eigen::Vector4d&) {PDPC_DEBUG_ASSERT(false); return "";}

template<typename T>
void IO::from_string(T& value, const std::string& str)
{
    PDPC_DEBUG_ASSERT(false);
    (void)(value);
    (void)(str);
}

void IO::from_string(bool& value, const std::string& str)
{
    value = str=="1" || str=="true";
}

void IO::from_string(int& value, const std::string& str) {value = std::stoi(str);}
void IO::from_string(uint& value, const std::string& str) {value = std::stol(str);}
void IO::from_string(float& value, const std::string& str) {value = std::stof(str);}
void IO::from_string(double& value, const std::string& str) {value = std::stod(str);}

void IO::from_string(std::string& value, const std::string& str)
{
    value = str;
}

} // namespace pdpc
