#include <PDPC/SpacePartitioning/Query/IndexQuery.h>

namespace pdpc {

IndexQuery::IndexQuery() :
    m_index(-1)
{
}

IndexQuery::IndexQuery(int index) :
    m_index(index)
{
}

int IndexQuery::index() const
{
    return m_index;
}

void IndexQuery::set_index(int index)
{
    m_index = index;
}

} // namespace pdpc
