#include "random.h"

namespace sqliteBench {

/*
 * https://github.com/google/leveldb/blob/master/util/testutil.cc
 */
char* RandomGenerator::random_string(Random* rnd, int len) {
  char* dst = static_cast<char*>(malloc(sizeof(char) * (size_t)(len + 1)));
  for (int i = 0; i < len; i++) {
    dst[i] = (char)(' ' + rand_uniform(95));
  }
  dst[len] = '\0';

  return dst;
}

char* RandomGenerator::compressible_string(Random* rnd, double compressed_fraction, size_t len) {

  int raw = (int)(len * compressed_fraction);
  if (raw < 1) raw = 1;
  char* raw_data = random_string(rnd, raw);
  size_t raw_data_len = strlen(raw_data);

  int pos = 0;
  char* dst = static_cast<char*>(malloc(sizeof(char) * (len + 1)));
  dst[0] = '\0';
  while (pos < len) {
    strcat(dst, raw_data);
    pos += raw_data_len;
  }

  return dst;
}

/*
 * https://github.com/google/leveldb/blob/master/util/random.h
 */
void RandomGenerator::rand_init(uint32_t s) {
  rand_.seed_ = s & 0x7fffffffu;
  /* Avoid bad seeds. */
  if (rand_.seed_ == 0 || rand_.seed_ == 2147483647L) {
    rand_.seed_ = 1;
  }
}

uint32_t RandomGenerator::rand_next() {
  static const uint32_t M = 2147483647L;
  static const uint64_t A = 16807;

  uint64_t product = rand_.seed_ * A;

  rand_.seed_ = (uint32_t)((product >> 31) + (product & M));

  if (rand_.seed_ > M) {
    rand_.seed_ -= M;
  }

  return rand_.seed_;
}

uint32_t RandomGenerator::rand_uniform(int n) { 
	return rand_next() % n; 
}

void RandomGenerator::rand_gen_init(double compression_ratio) {
  Random rnd;
  char* piece;
  
  data_ = static_cast<char*>(malloc(sizeof(char) * 1048576));
  data_size_ = 0;
  pos_ = 0;
  (data_)[0] = '\0';

  rand_init(301);
  while (data_size_ < 1048576) {
    piece = compressible_string(&rnd, compression_ratio, 100);
    strcat(data_, piece);
    data_size_ += strlen(piece);
  }

  free(piece);
}

char* RandomGenerator::rand_gen_generate(int len) {
  if (pos_ + len > data_size_) {
    pos_ = 0;
    assert(len < data_size_);
  }
  pos_ += len;
  char* substr = static_cast<char*>(malloc(sizeof(char) * (len + 1)));
  strncpy(substr, (data_) + pos_ - len, len);

  return substr;
}
} // namespace sqliteBench
