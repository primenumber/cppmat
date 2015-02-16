#pragma once

#include <cassert>

#include <type_traits>

template <typename T>
struct is_matrix {
  constexpr static bool value = false;
};

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
T &operator+=(T &lhs, const T &rhs) {
  assert(cols(lhs) == cols(rhs) && rows(lhs) == rows(rhs));
  for (size_t i = 0; i < size(lhs); ++i)
    lhs.sequence(i) += rhs.sequence(i);
  return lhs;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
T &operator-=(T &lhs, const T &rhs) {
  assert(cols(lhs) == cols(rhs) && rows(lhs) == rows(rhs));
  for (size_t i = 0; i < size(lhs); ++i)
    lhs.sequence(i) -= rhs.sequence(i);
  return lhs;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
constexpr T operator+(const T &lhs, const T &rhs) {
  assert(cols(lhs) == cols(rhs) && rows(lhs) == rows(rhs));
  T res(lhs);
  return res += rhs;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
T operator-(const T &lhs, const T &rhs) {
  assert(cols(lhs) == cols(rhs) && rows(lhs) == rows(rhs));
  T res(lhs);
  return res -= rhs;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
T operator-(const T &mat) {
  T res(rows(mat), cols(mat));
  for (size_t i = 0; i < size(mat); ++i)
    res.sequence(i) = -mat.sequence(i);
  return res;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr>
T operator*(const T &lhs, const T &rhs) {
  assert(cols(lhs) == rows(rhs));
  T res(rows(lhs), cols(rhs));
  res.clear_zero();
  for (size_t i = 0; i < rows(lhs); ++i) {
    for (size_t k = 0; k < cols(lhs); ++k) {
      for (size_t j = 0; j < cols(rhs); ++j) {
        res[i][j] += lhs[i][k] * rhs[k][j];
      }
    }
  }
  return res;
}

template <typename T,
         typename std::enable_if<is_matrix<T>::value>::type* = nullptr,
         typename U,
         typename std::enable_if<std::is_integral<U>::value>::type* = nullptr>
T operator^(const T &mat, const U &index) {
  assert(index >= 0);
  assert(rows(mat) == cols(mat));
  if (index == 0) return T(1, rows(mat), cols(mat));
  else if (index == 1) return mat;
  T half = mat ^ (index / 2);
  if ((index % 2) == 1) {
    return mat * half * half;
  } else {
    return half * half;
  }
}
