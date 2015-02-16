#include <iostream>
#include <random>

#include <boost/timer.hpp>

#include "matrix.hpp"

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "few parameters" << std::endl;
    return 1;
  }
  size_t n = atoi(argv[1]);
  std::random_device rd;
  std::mt19937 mt(rd());
  std::normal_distribution<> d(0, 10);
  matrix<double> A(n,n), B(n,n);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      A[i][j] = d(mt);
      B[i][j] = d(mt);
    }
  }
  boost::timer t;
  auto C = A * B;
  std::cout << t.elapsed() << " sec" << std::endl;
  double tr = 0.0;
  for (size_t i = 0; i < n; ++i) tr += C[i][i];
  std::cout << tr << std::endl;
  return 0;
}
