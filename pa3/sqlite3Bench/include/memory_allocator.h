#ifndef MEMORY_ALLOCATOR_H_
#define MEMORY_ALLOCATOR_H_

#include <cstddef>
#include <cassert>
#include <iostream>
#include <string.h>
#include <stdlib.h>

namespace sqliteBench {

#define kNumBuckets 154
#define kNumData 1000000

class Memory_allocator {

  double *data_;
  size_t data_size_;
  int pos_;

  public:
    Memory_allocator() : data_(NULL), data_size_(0), pos_(0) {}
    void raw_calloc();
    void raw_realloc();
    void raw_clear();
    void raw_add(double value);
    char* raw_to_string();
    void raw_print(FILE *stream);
};

} // namespace sqliteBench
#endif
