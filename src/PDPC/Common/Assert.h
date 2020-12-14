#pragma once

#include <PDPC/Common/Macro.h>

#define PDPC_ERROR                                                             \
    PDPC_MACRO_START                                                           \
    PDPC_PRINT_ERROR("");                                                      \
    PDPC_CRASH;                                                                \
    PDPC_MACRO_END

#define PDPC_ERROR_MSG(MSG)                                                    \
    PDPC_MACRO_START                                                           \
    PDPC_PRINT_ERROR(MSG);                                                     \
    PDPC_CRASH;                                                                \
    PDPC_MACRO_END

#define PDPC_ASSERT(EXPR)                                                      \
    PDPC_MACRO_START                                                           \
    if(!(EXPR)) {                                                              \
        PDPC_ERROR_MSG(PDPC_STR(EXPR));                                        \
    }                                                                          \
    PDPC_MACRO_END

#define PDPC_ASSERT_MSG(EXPR,MSG)                                              \
    PDPC_MACRO_START                                                           \
    if(!(EXPR)) {                                                              \
        PDPC_ERROR_MSG(MSG);                                                   \
    }                                                                          \
    PDPC_MACRO_END

#ifdef PDPC_DEBUG
    #define PDPC_DEBUG_ASSERT(EXPR)          PDPC_ASSERT(EXPR)
    #define PDPC_DEBUG_ASSERT_MSG(EXPR,MSG)  PDPC_ASSERT_MSG(EXPR,MSG)
    #define PDPC_DEBUG_ERROR_MSG(MSG)        PDPC_ERROR_MSG(MSG)
    #define PDPC_DEBUG_ERROR                 PDPC_ERROR
#else
    #define PDPC_DEBUG_ASSERT(EXPR)
    #define PDPC_DEBUG_ASSERT_MSG(EXPR,MSG)
    #define PDPC_DEBUG_ERROR_MSG(MSG)
    #define PDPC_DEBUG_ERROR
#endif
