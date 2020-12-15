#pragma once

#include <string>

namespace pdpc {
namespace str {

//! \brief add n '0' in front of s
std::string push_zeros_front(const std::string& s, int n);

//! \brief int to string using n digits
std::string to_string(int number, int n);
std::string to_string(const std::string& s, int n);

} // namespace str
} // namespace pdpc
