#pragma once

#include <cstdlib>

constexpr size_t BLOCK_SIZE = 64;
constexpr size_t SIMD_ALIGNMENT = 32;

template <typename T>
T *simd_aligned_alloc(size_t n) {
  T *ptr;
  int errer_number = posix_memalign(reinterpret_cast<void **>(&ptr),
      SIMD_ALIGNMENT, n * sizeof(T));
  if (errer_number != 0) throw std::bad_alloc();
  return ptr;
}

constexpr size_t block_roundup(size_t n) {
  return (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

