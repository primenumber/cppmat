#pragma once

#include "matrix_operations.hpp"
#include "blocked_matrix.hpp"

template <typename T>
class matrix {
 public:
  matrix(const matrix<T>&) = default;
  matrix(matrix<T> &&) = default;
  matrix(const size_t rows, const size_t cols, const blocked_matrix<T>& b_mat) :
      rows(rows), cols(cols), b_mat(b_mat) {};
  matrix(const size_t rows, const size_t cols, blocked_matrix<T> && b_mat) :
      rows(rows), cols(cols), b_mat(std::move(b_mat)) {};
  matrix(const size_t rows, const size_t cols) : matrix(0, rows, cols) {}
  template <typename U,
           typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr>
  matrix(const U &scalar, const size_t rows, const size_t cols) :
      rows(rows), cols(cols),
      b_mat(scalar, block_roundup(rows), block_roundup(cols)) {}
  ~matrix() = default;
  class accessor {
   public:
    constexpr accessor(matrix<T> &mat, const size_t index_r) :
        mat(mat), index_r(index_r) {}
    constexpr T &operator[](const size_t index_c) {
      size_t bindex_r = index_r / BLOCK_SIZE;
      size_t bindex_c = index_c / BLOCK_SIZE;
      size_t iindex_r = index_r % BLOCK_SIZE;
      size_t iindex_c = index_c % BLOCK_SIZE;
      return mat.b_mat[bindex_r][bindex_c][iindex_r][iindex_c];
    }
   private:
    matrix<T> &mat;
    const size_t index_r;
  };
  friend accessor;
  accessor operator[](const size_t index_r) {
    return accessor(*this, index_r);
  }
  template <typename U>
  friend matrix<U> &operator+=(matrix<U> &, const matrix<U> &);
  template <typename U>
  friend matrix<U> &operator-=(matrix<U> &, const matrix<U> &);
  template <typename U>
  friend matrix<U> operator+(const matrix<U> &, const matrix<U> &);
  template <typename U>
  friend matrix<U> operator-(const matrix<U> &, const matrix<U> &);
  template <typename U>
  friend matrix<U> operator-(const matrix<U> &);
  template <typename U>
  friend matrix<U> operator*(const matrix<U> &, const matrix<U> &);
  template <typename U>
  friend size_t size(const matrix<T> &);
  const size_t rows, cols;
 private:
  blocked_matrix<T> b_mat;
};

template <typename T>
struct is_matrix<matrix<T>> {
  constexpr static bool value = true;
};

template <typename T>
matrix<T> &operator+=(matrix<T> &lhs, const matrix<T> &rhs) {
  lhs.b_mat += rhs.b_mat;
  return lhs;
}

template <typename T>
matrix<T> &operator-=(matrix<T> &lhs, const matrix<T> &rhs) {
  lhs.b_mat -= rhs.b_mat;
  return lhs;
}

template <typename T>
matrix<T> operator+(const matrix<T> &lhs, const matrix<T> &rhs) {
  return matrix<T>(rows(lhs), cols(lhs), lhs.b_mat + rhs.b_mat);
}

template <typename T>
matrix<T> operator-(const matrix<T> &lhs, const matrix<T> &rhs) {
  return matrix<T>(rows(lhs), cols(lhs), lhs.b_mat - rhs.b_mat);
}

template <typename T>
matrix<T> operator-(const matrix<T> &mat) {
  return matrix<T>(rows(mat), cols(mat), -mat.b_mat);
}

template <typename T>
matrix<T> operator*(const matrix<T> &lhs, const matrix<T> &rhs) {
  return matrix<T>(rows(lhs), cols(rhs), lhs.b_mat * rhs.b_mat);
}

template <typename T>
size_t size(const matrix<T> &mat) {
  return mat.rows * mat.cols;
}

template <typename T>
size_t rows(const matrix<T> &mat) {
  return mat.rows;
}

template <typename T>
size_t cols(const matrix<T> &mat) {
  return mat.cols;
}
