#include "bench.h"
#include <sys/time.h>

namespace sqliteBench {

uint64_t now_micros() {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return (uint64_t)(tv.tv_sec * 1000000 + tv.tv_usec);
}

/*
 * https://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c 
 */
bool starts_with(const char* str, const char* pre) {
  size_t lenpre = strlen(pre);
  size_t lenstr = strlen(str);

  return lenstr < lenpre ? false : !strncmp(pre, str, lenpre);
}

char* trim_space(const char* s) {
  size_t start = 0;
  while (start < strlen(s) && isspace(s[start])) {
    start++;
  }
  size_t limit = strlen(s);
  while (limit > start && isspace(s[limit - 1])) {
    limit--;
  }

  char* res = static_cast<char*>(calloc(sizeof(char), limit - start + 1));
  for (size_t i = 0; i < limit - start; i++)
    res[i] = s[start + i];
  res[limit - start] = '\0';

  return res;
}
} // namespace sqliteBench
