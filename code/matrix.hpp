#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cassert>

#include <initializer_list>
#include <memory>
#include <ostream>

template <class T>
class Matrix {
 public:
  class Invalid {};

  Matrix(std::size_t nrows, std::size_t ncols) : Matrix(nrows, ncols, T()) {}

  Matrix(std::size_t nrows, std::size_t ncols, T init)
      : nrows_(nrows),
        ncols_(ncols),
        data_(std::make_unique<T[]>(nrows * ncols)) {
    if (nrows == 0 || ncols == 0) {
      throw Invalid{};
    }
    for (std::size_t i = 0, end = nrows * ncols; i < end; ++i) {
      data_[i] = init;
    }
  }

  Matrix(std::initializer_list<std::initializer_list<T>> elements)
      : nrows_(elements.size()), ncols_(0), data_() {
    if (elements.size() == 0 || elements.begin()->size() == 0) {
      throw Invalid{};
    }
    ncols_ = elements.begin()->size();
    data_ = std::make_unique<T[]>(nrows_ * ncols_);

    std::size_t i = 0;
    for (auto row : elements) {
      // the number of columns must be the same for each row
      if (row.size() != ncols_) {
        throw Invalid{};
      }
      for (auto e : row) {
        data_[i] = e;
        ++i;
      }
    }
  }

  Matrix(const Matrix& m) : nrows_(m.nrows()), ncols_(m.ncols()), data_() {
    data_ = std::make_unique<T[]>(nrows_ * ncols_);
    std::copy(m.data_.get(), m.data_.get() + (nrows_ * ncols_),
              data_.get());
  }

  Matrix& operator=(const Matrix& m) {
    // self-assignment, don't do anything
    if (this == &m) {
      return *this;
    }
    // there is not enough space in our current matrix, get enough
    if (nrows_ * ncols_ < m.nrows() * m.ncols()) {
      data_ = std::make_unique<T[]>(m.nrows() * m.ncols());
    }
    nrows_ = m.nrows();
    ncols_ = m.ncols();
    std::copy(m.data_.get(), m.data_.get() + (nrows_ * ncols_),
              data_.get());
    return *this;
  }

  T& operator()(std::size_t i, std::size_t j) {
    assert(i * ncols_ + j < nrows_ * ncols_);
    return data_[i * ncols_ + j];
  }

  T operator()(std::size_t i, std::size_t j) const {
    assert(i * ncols_ + j < nrows_ * ncols_);
    return data_[i * ncols_ + j];
  }

  std::size_t nrows() const { return nrows_; }

  std::size_t ncols() const { return ncols_; }

 private:
  std::size_t nrows_;
  std::size_t ncols_;
  std::unique_ptr<T[]> data_;
};

/*
 * Row-wise space separated output of the matrix
 *
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
  for (std::size_t i = 0; i < m.nrows(); ++i) {
    for (std::size_t j = 0; j < m.ncols() - 1; ++j) {
      os << m(i, j) << ' ';
    }
    os << m(i, m.ncols() - 1) << '\n';
  }
  return os;
}

#endif  // MATRIX_HPP

