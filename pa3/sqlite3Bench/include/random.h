#ifndef RANDOM_H_
#define RANDOM_H_

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

namespace sqliteBench {

typedef struct Random {
  uint32_t seed_;
} Random;


class RandomGenerator {
  private:
    char* data_;
    size_t data_size_;
    int pos_;
    Random rand_;

  public:

    char* random_string(Random* rnd, int len);
		char* compressible_string(Random* rnd, double compressed_fraction, size_t len);

    void rand_init(uint32_t);
    uint32_t rand_next();
    uint32_t rand_uniform(int);
    void rand_gen_init(double);
    char* rand_gen_generate(int);
};
} // namespace sqliteBench

#endif
