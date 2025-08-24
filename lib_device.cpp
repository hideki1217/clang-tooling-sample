#include <cstdio>

#include "lib.hpp"

__device__ void log(const char *message) { std::printf("%s", message); }

__global__ void aaaaa(int, int, int, int, int, int, int,
                      double (*)(double, double)) {}

__global__ int sub(int x, int y) { return x - y; }
__global__ int mul(int x, int y) { return x * y; }
__global__ int muladd(int n, int *x, int *y) {
  log("muladd called");
  int reg = 0;
  for (int i = 0; i < n; ++i) {
    reg = add(reg, mul(x[i], y[i]));
  }
  return reg;
}