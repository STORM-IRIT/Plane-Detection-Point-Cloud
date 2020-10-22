#pragma once

namespace pdpc {

class IndexQuery
{
public:
    IndexQuery();
    IndexQuery(int index);

    int index() const;
    void set_index(int index);

protected:
    int m_index;
};

} // namespace pdpc
