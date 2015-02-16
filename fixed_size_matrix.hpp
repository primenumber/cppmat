#pragma once

#include <type_traits>

#include "matrix_operations.hpp"
#include "utility.hpp"

template <typename T>
struct fixed_size_matrix {
  using base_type = T;
  constexpr static size_t MATRIX_SIZE = BLOCK_SIZE;
  constexpr static size_t MATRIX_ELEMENTS = MATRIX_SIZE * MATRIX_SIZE;
  constexpr static size_t MATRIX_BYTES = MATRIX_ELEMENTS * sizeof(T);
  constexpr fixed_size_matrix() = default;
  constexpr fixed_size_matrix(size_t, size_t) {};
  constexpr fixed_size_matrix(const fixed_size_matrix<T> &) = default;
  constexpr fixed_size_matrix(fixed_size_matrix<T> &&) = default;
  template <typename U,
           typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr>
  constexpr fixed_size_matrix(const U &scalar) {
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
      for (size_t j = 0; j < MATRIX_SIZE; ++j) {
        values[i*MATRIX_SIZE+j] = ((i == j) ? T(scalar) : T(0));
      }
    }
  }
  template <typename U,
           typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr>
  constexpr fixed_size_matrix(const U &scalar, size_t, size_t) : fixed_size_matrix<T>(scalar) {}
  constexpr fixed_size_matrix(const T * const src) {
    for (size_t i = 0; i < MATRIX_ELEMENTS; ++i)
      values[i] = src[i];
  }
  fixed_size_matrix<T> &operator=(const fixed_size_matrix<T> &) = default;
  fixed_size_matrix<T> &operator=(fixed_size_matrix<T> &&) = default;
  constexpr T * operator[](const size_t index) {
    return values + index * MATRIX_SIZE;
  }
  constexpr const T * const operator[](const size_t index) const {
    return values + index * MATRIX_SIZE;
  }
  constexpr T * data() { return values; }
  constexpr const T * const data() const { return values; }
  constexpr T &sequence(size_t index) { return values[index]; }
  constexpr const T &sequence(size_t index) const { return values[index]; }
  void clear_zero() {
    for (size_t i = 0; i < MATRIX_ELEMENTS; ++i)
      values[i] = T(0);
  }
 private:
  alignas(SIMD_ALIGNMENT) T values[MATRIX_ELEMENTS];
};

template <typename T>
struct is_matrix<fixed_size_matrix<T>> {
  constexpr static bool value = true;
};

template <typename T>
constexpr size_t size(const fixed_size_matrix<T> &) {
  return fixed_size_matrix<T>::MATRIX_ELEMENTS;
}

template <typename T>
constexpr size_t rows(const fixed_size_matrix<T> &) {
  return fixed_size_matrix<T>::MATRIX_SIZE;
}

template <typename T>
constexpr size_t cols(const fixed_size_matrix<T> &) {
  return fixed_size_matrix<T>::MATRIX_SIZE;
}
