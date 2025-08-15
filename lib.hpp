#pragma once

#define __global__ __attribute__((annotate("special")))

__global__ extern int add(int x, int y);