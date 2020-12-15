#include <PDPC/Common/String.h>

namespace pdpc {
namespace str {

std::string push_zeros_front(const std::string& s, int n)
{
    if(n > 0)
    {
        return std::string(n, '0').append(s);
    }
    else
    {
        return s;
    }
}

std::string to_string(int number, int n)
{
    return to_string(std::to_string(number), n);
}

std::string to_string(const std::string& s, int n)
{
    const int diff = n - s.size();
    return push_zeros_front(s, diff);
}

} // namespace str
} // namespace pdpc
