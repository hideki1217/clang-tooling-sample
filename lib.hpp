#pragma once

#define __global__ __attribute__((annotate("special")))
#define __device__

__global__ extern int add(int x, int y);