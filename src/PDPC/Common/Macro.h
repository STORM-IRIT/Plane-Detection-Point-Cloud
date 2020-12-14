#pragma once

#include <cstdio>

// macro delimiters
#define PDPC_MACRO_START do {
#define PDPC_MACRO_END   } while(0)

// Stringification
#define PDPC_XSTR(S) #S
#define PDPC_STR(S) PDPC_XSTR(S)

#define PDPC_CRASH                                                             \
    PDPC_MACRO_START                                                           \
    asm volatile ("int $3");                                                   \
    PDPC_MACRO_END

#define PDPC_PRINT_ERROR(MSG)                                                  \
    PDPC_MACRO_START                                                           \
    fprintf(stderr,                                                            \
            "%s:%i: [Error] %s\n",                                             \
            __FILE__,__LINE__,MSG);                                            \
    fflush(stderr);                                                            \
    PDPC_MACRO_END

#define PDPC_PRINT_WARNING(MSG)                                                \
    PDPC_MACRO_START                                                           \
    fprintf(stderr,                                                            \
            "%s:%i: [Warning] %s\n",                                           \
            __FILE__,__LINE__,MSG);                                            \
    fflush(stderr);                                                            \
    PDPC_MACRO_END

// turnoff warning
#define PDPC_UNUSED(VAR)                                                       \
    PDPC_MACRO_START                                                           \
    (void)(VAR);                                                               \
    PDPC_MACRO_END

#define PDPC_TODO                                                              \
    PDPC_MACRO_START                                                           \
    PDPC_PRINT_ERROR("TODO");                                                  \
    PDPC_CRASH;                                                                \
    PDPC_MACRO_END

template<typename T> struct XXXXXXXXXX_The_unkown_type_is_;
#define WhatIsTheTypeOf(expr) XXXXXXXXXX_The_unkown_type_is_<decltype(expr)> _XXXXXXXXXX
#define WhatIsThisType(type) XXXXXXXXXX_The_unkown_type_is_<type> _XXXXXXXXXX
