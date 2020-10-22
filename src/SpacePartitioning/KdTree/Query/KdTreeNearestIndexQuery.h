#pragma once

#include <Query/NearestIndexQuery.h>
#include <KdTree/Query/KdTreeQuery.h>
#include <KdTree/Iterator/KdTreeNearestIndexIterator.h>

namespace pdpc {

class KdTreeNearestIndexQuery : public KdTreeQuery,
                                public NearestIndexQuery
{
public:
    KdTreeNearestIndexQuery();
    KdTreeNearestIndexQuery(const KdTree* kdtree);
    KdTreeNearestIndexQuery(const KdTree* kdtree, int index);

public:
    KdTreeNearestIndexIterator begin();
    KdTreeNearestIndexIterator end();

public:
    const NearestIndexQuery& search();
};

} // namespace pdpc
