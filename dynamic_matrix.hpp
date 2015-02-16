#pragma once

#include <algorithm>

#include "matrix_operations.hpp"
#include "fixed_size_matrix.hpp"

template <typename T>
class dynamic_matrix {
 public:
  using base_type = T;
  dynamic_matrix(const dynamic_matrix<T> &that) :
      rows(that.rows), cols(that.cols),
      values(simd_aligned_alloc<T>(rows * cols)) {
    std::copy(that.values, that.values + rows * cols, values);
  }
  dynamic_matrix(dynamic_matrix<T> &&that) :
      rows(that.rows), cols(that.cols),
      values(that.values) {
    that.enable = false;
  }
  dynamic_matrix(const size_t rows, const size_t cols) :
      rows(rows), cols(cols),
      values(simd_aligned_alloc<T>(rows * cols)) {
    std::fill(values, values + rows * cols, T(0));
  }
  template <typename U,
           typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr>
  dynamic_matrix(const U &scalar, const size_t rows, const size_t cols) :
      rows(rows), cols(cols),
      values(simd_aligned_alloc<T>(rows * cols)) {
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
        values[i * cols + j] = ((i == j) ? T(scalar) : T(0));
      }
    }
  }
  ~dynamic_matrix() {
    if (enable) {
      free(values);
    }
  }
  constexpr T * operator[](const size_t index) { return values + index * cols; }
  constexpr const T * const operator[](const size_t index) const { return values + index * cols; }
  constexpr T * data() { return values; }
  constexpr const T * const data() const { return values; }
  constexpr T &sequence(size_t index) { return values[index]; }
  constexpr const T &sequence(size_t index) const { return values[index]; }
  void clear_zero() {
    std::fill(values, values + rows * cols, T(0));
  }
  const size_t rows, cols;
 private:
  T * const values;
  bool enable = true;
};

template <typename T>
struct is_matrix<dynamic_matrix<T>> {
  constexpr static bool value = true;
};

template <typename T>
constexpr size_t size(const dynamic_matrix<T> &mat) {
  return mat.rows * mat.cols;
}

template <typename T>
constexpr size_t rows(const dynamic_matrix<T> &mat) {
  return mat.rows;
}

template <typename T>
constexpr size_t cols(const dynamic_matrix<T> &mat) {
  return mat.cols;
}

dynamic_matrix<double> operator*(const dynamic_matrix<double> &lhs,
    const dynamic_matrix<double> &rhs) {
  assert(cols(lhs) == rows(rhs));
  dynamic_matrix<double> res(0, rows(lhs), cols(rhs));
  const size_t ir = (rows(lhs)/BLOCK_SIZE)*BLOCK_SIZE;
  const size_t kr = (cols(lhs)/BLOCK_SIZE)*BLOCK_SIZE;
  const size_t jr = (cols(rhs)/BLOCK_SIZE)*BLOCK_SIZE;
  for (size_t i = 0; i < rows(lhs) - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
    for (size_t k = 0; k < cols(lhs) - BLOCK_SIZE + 1; k += BLOCK_SIZE) {
      for (size_t j = 0; j < cols(rhs) - BLOCK_SIZE + 1; j += BLOCK_SIZE) {
        for (size_t ii = 0; ii < BLOCK_SIZE; ++ii) {
          const auto res_p = res[i+ii];
          for (size_t kk = 0; kk < BLOCK_SIZE; ++kk) {
            const double lhs_v = lhs[i+ii][k+kk];
            const auto rhs_p = rhs[k+kk];
            for (size_t jj = 0; jj < BLOCK_SIZE; ++jj) {
              const size_t jpjj = j + jj;
              res_p[jpjj] += lhs_v * rhs_p[jpjj];
            }
          }
        }
      }
      for (size_t j = 0; j < cols(rhs) % BLOCK_SIZE; ++j) {
        for (size_t ii = 0; ii < BLOCK_SIZE; ++ii) {
          for (size_t kk = 0; kk < BLOCK_SIZE; ++kk) {
            res[i+ii][jr+j] += lhs[i+ii][k+kk] * rhs[k+kk][jr+j];
          }
        }
      }
    }
    for (size_t k = 0; k < cols(lhs) % BLOCK_SIZE; ++k) {
      for (size_t j = 0; j < cols(rhs) - BLOCK_SIZE + 1; j += BLOCK_SIZE) {
        for (size_t ii = 0; ii < BLOCK_SIZE; ++ii) {
          for (size_t jj = 0; jj < BLOCK_SIZE; ++jj) {
            res[i+ii][j+jj] += lhs[i+ii][kr+k] * rhs[kr+k][j+jj];
          }
        }
      }
      for (size_t j = 0; j < cols(rhs) % BLOCK_SIZE; ++j) {
        for (size_t ii = 0; ii < BLOCK_SIZE; ++ii) {
          res[i+ii][jr+j] += lhs[i+ii][kr+k] * rhs[kr+k][jr+j];
        }
      }
    }
  }
  for (size_t i = 0; i < rows(lhs) % BLOCK_SIZE; ++i) {
    for (size_t k = 0; k < cols(lhs) - BLOCK_SIZE + 1; k += BLOCK_SIZE) {
      for (size_t j = 0; j < cols(rhs) - BLOCK_SIZE + 1; j += BLOCK_SIZE) {
        for (size_t kk = 0; kk < BLOCK_SIZE; ++kk) {
          for (size_t jj = 0; jj < BLOCK_SIZE; ++jj) {
            res[ir+i][j+jj] += lhs[ir+i][k+kk] * rhs[k+kk][j+jj];
          }
        }
      }
      for (size_t j = 0; j < cols(rhs) % BLOCK_SIZE; ++j) {
        for (size_t kk = 0; kk < BLOCK_SIZE; ++kk) {
          res[ir+i][jr+j] += lhs[ir+i][k+kk] * rhs[k+kk][jr+j];
        }
      }
    }
    for (size_t k = 0; k < cols(lhs) % BLOCK_SIZE; ++k) {
      for (size_t j = 0; j < cols(rhs) - BLOCK_SIZE + 1; j += BLOCK_SIZE) {
        for (size_t jj = 0; jj < BLOCK_SIZE; ++jj) {
          res[ir+i][j+jj] += lhs[ir+i][kr+k] * rhs[kr+k][j+jj];
        }
      }
      for (size_t j = 0; j < cols(rhs) % BLOCK_SIZE; ++j) {
        res[ir+i][jr+j] += lhs[ir+i][kr+k] * rhs[kr+k][jr+j];
      }
    }
  }
  return res;
}
