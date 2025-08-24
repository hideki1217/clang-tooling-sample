#pragma once

#ifdef __clang__
#define __global__ extern "C" __attribute__((annotate("special")))
#define __device__
#else
#define __global__ extern "C"
#define __device__
#endif

__global__ int add(int x, int y);