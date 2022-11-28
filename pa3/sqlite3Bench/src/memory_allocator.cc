#include "memory_allocator.h"

namespace sqliteBench {

void Memory_allocator::raw_calloc() {
  data_size_ = kNumData;
  data_ = static_cast<double*>(calloc(sizeof(double), data_size_));
  pos_ = 0;
}

void Memory_allocator::raw_realloc() {
  data_size_ *= 2;
  data_ = static_cast<double*>(realloc(data_, sizeof(double) * data_size_));
  if (!data_) {
    fprintf(stderr, "realloc failed\n");
    exit(1);
  }
}

void Memory_allocator::raw_clear() {
  if (data_) {
    free(data_);
  }
  raw_calloc();
}

void Memory_allocator::raw_add(double value) {
  if (!data_)
    raw_calloc();
  if (data_size_ < pos_ + 1)
    raw_realloc();
  data_[pos_] = value;
  pos_++;
}

char* Memory_allocator::raw_to_string() {
  if (!data_)
    raw_calloc();
  size_t r_size = 1024;
  char *r = static_cast<char*>(malloc(sizeof(char) * r_size));
  strcpy(r, "");
  char buf[200];
  for (int i = 0; i < pos_; i++) {
    snprintf(buf, sizeof(buf), "%.4f\n", data_[i]);
    if (r_size < strlen(r) + strlen(buf)) {
      r = static_cast<char*>(realloc(r, r_size * 2));
      r_size *= 2;
    }
    strcat(r, buf);
  }
  return r;
}

void Memory_allocator::raw_print(FILE *stream) {
  if (!data_) {
    raw_calloc();
  }
  fprintf(stream, "num,time\n");
  for (int i = 0; i < pos_; i++)
    fprintf(stream, "%d,%.4f\n", i, data_[i]);
}

} // namespace sqliteBench
