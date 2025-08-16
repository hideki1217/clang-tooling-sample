#include <iostream>

#include "lib.hpp"

__global__ void aaaaa(int, int, int, int, int, int, int,
                      double (*)(double, double));

__global__ int sub(int x, int y) { return x - y; }
__global__ int mul(int x, int y) { return x * y; }
__global__ int muladd(int n, int *x, int *y) {
  int reg = 0;
  for (int i = 0; i < n; ++i) {
    reg = add(reg, mul(x[i], y[i]));
  }
  return reg;
}

int main() {
  int x = 10;
  int y = 3;
  std::cout << x << " + " << y << "=" << add(x, y) << "\n";
  std::cout << x << " - " << y << "=" << sub(x, y) << "\n";
}