/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#pragma once

////////////////////////////////////////////////////////////////////////////////
// Compatibility types, macros, functions
//
#ifdef __CUDACC__
# include <cuda.h>
# define PONCA_MULTIARCH __host__ __device__
#else
# define PONCA_MULTIARCH

// GCC: compile with -std=c++0x
# if defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || (__GNUC__ >= 5))
#   if defined(nullptr_t) || (__cplusplus > 199711L) || defined(HACK_GCC_ITS_CPP0X)
#     define __CPP0X__
#   endif
# endif

#endif // ifdef __CUDACC__

#ifdef __CUDA_ARCH__
  #define PONCA_MULTIARCH_INCLUDE_STD(FILENAME) "defines.h"
  #define PONCA_MULTIARCH_STD_MATH(FUNC)
  #define PONCA_CUDA_ARCH
#else
  #define PONCA_MULTIARCH_INCLUDE_STD(FILENAME) <FILENAME>
  #define PONCA_MULTIARCH_STD_MATH(FUNC) using std::FUNC;
  #define PONCA_CPU_ARCH
#endif
