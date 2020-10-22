#include <PDPC/Common/Containers/internal/AbstractVector.h>

namespace pdpc {


// =============================================================================

// explicit template instanciation
template class concrete_vector<int>;
template class concrete_vector<float>;
template class concrete_vector<double>;
template class concrete_vector<uint>;
template class concrete_vector<std::string>;

} // namespace pdpc
