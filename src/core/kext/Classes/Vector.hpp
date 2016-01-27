#ifndef VECTOR_HPP
#define VECTOR_HPP

#define DECLARE_VECTOR_CLASS_BODY(TYPENAME, CHUNKSIZE)                  \
public:                                                                 \
  Vector_##TYPENAME(void) : vector_(nullptr), capacity_(0), size_(0) {} \
                                                                        \
  ~Vector_##TYPENAME(void) {                                            \
    if (vector_) {                                                      \
      delete[] vector_;                                                 \
    }                                                                   \
  }                                                                     \
                                                                        \
  void reserve(size_t n) {                                              \
    if (n <= capacity_) return;                                         \
                                                                        \
    TYPENAME* p = new TYPENAME[n];                                      \
    if (p) {                                                            \
      capacity_ = n;                                                    \
      for (size_t i = 0; i < size_; ++i) {                              \
        p[i] = vector_[i];                                              \
      }                                                                 \
                                                                        \
      if (vector_) {                                                    \
        delete[] vector_;                                               \
      }                                                                 \
      vector_ = p;                                                      \
    }                                                                   \
  }                                                                     \
                                                                        \
  void clear(void) {                                                    \
    if (vector_) {                                                      \
      delete[] vector_;                                                 \
      vector_ = nullptr;                                                \
    }                                                                   \
    capacity_ = 0;                                                      \
    size_ = 0;                                                          \
  }                                                                     \
                                                                        \
  Vector_##TYPENAME& push_back(const TYPENAME& newval) {                \
    if (size_ >= capacity_) {                                           \
      reserve(capacity_ + CHUNKSIZE);                                   \
    }                                                                   \
                                                                        \
    if (size_ < capacity_) {                                            \
      vector_[size_] = newval;                                          \
      ++size_;                                                          \
    }                                                                   \
                                                                        \
    return *this;                                                       \
  }                                                                     \
                                                                        \
  size_t size(void) const { return size_; }                             \
  size_t capacity(void) const { return capacity_; }                     \
  bool empty(void) const { return size_ == 0; }                         \
                                                                        \
  Vector_##TYPENAME& operator=(const Vector_##TYPENAME& rhs) {          \
    if (this == &rhs) return *this;                                     \
                                                                        \
    clear();                                                            \
    capacity_ = rhs.capacity_;                                          \
    size_ = rhs.size_;                                                  \
    if (size_ == 0) {                                                   \
      vector_ = nullptr;                                                \
    } else {                                                            \
      vector_ = new TYPENAME[capacity_];                                \
      for (size_t i = 0; i < size_; ++i) {                              \
        vector_[i] = rhs.vector_[i];                                    \
      }                                                                 \
    }                                                                   \
                                                                        \
    return *this;                                                       \
  }                                                                     \
  TYPENAME& operator[](size_t n) { return vector_[n]; }                 \
  const TYPENAME& operator[](size_t n) const { return vector_[n]; }     \
  TYPENAME& front(void) { return vector_[0]; }                          \
  TYPENAME& back(void) { return vector_[size_ - 1]; }                   \
                                                                        \
private:                                                                \
  Vector_##TYPENAME(const Vector_##TYPENAME& rhs) {}                    \
                                                                        \
  TYPENAME* vector_;                                                    \
  size_t capacity_;                                                     \
  size_t size_;

#define DECLARE_VECTOR_WITH_CHUNKSIZE(TYPENAME, CHUNKSIZE) \
  class Vector_##TYPENAME final {                          \
    DECLARE_VECTOR_CLASS_BODY(TYPENAME, CHUNKSIZE)         \
  };

#define DECLARE_VECTOR_WITH_CHUNKSIZE_WITH_HELPER(TYPENAME, CHUNKSIZE) \
  class Vector_##TYPENAME final {                                      \
    DECLARE_VECTOR_CLASS_BODY(TYPENAME, CHUNKSIZE)                     \
  public:                                                              \
    bool is_include(const TYPENAME& v) const {                         \
      for (size_t i = 0; i < size_; ++i) {                             \
        if (vector_[i] == v) {                                         \
          return true;                                                 \
        }                                                              \
      }                                                                \
      return false;                                                    \
    }                                                                  \
  };

#define DECLARE_VECTOR(TYPENAME) DECLARE_VECTOR_WITH_CHUNKSIZE(TYPENAME, 16)
#define DECLARE_VECTOR_WITH_HELPER(TYPENAME) DECLARE_VECTOR_WITH_CHUNKSIZE_WITH_HELPER(TYPENAME, 16)

DECLARE_VECTOR_WITH_HELPER(int);

#endif
