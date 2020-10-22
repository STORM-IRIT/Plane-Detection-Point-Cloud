#pragma once

namespace pdpc {

struct KdTreeNode
{
    union {
        struct {
            float          splitValue;
            unsigned int   firstChildId:24;
            unsigned int   dim:2;
            unsigned int   leaf:1;
        };
        struct {
            unsigned int   start;
            unsigned short size;
        };
    };
};

} // namespace pdpc
