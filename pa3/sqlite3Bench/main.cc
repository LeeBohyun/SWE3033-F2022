#include <cstdio>
#include <cstdlib>
#include "histogram.h"
#include "random.h"
#include "bench.h"

//namespace sqliteBench {
using namespace sqliteBench; 

void init() {
  // Comma-separated list of operations to run in the specified order
  //   Actual benchmarks:
  //
  //   fillseq       -- write N values in sequential key order in async mode
  //   fillseqsync   -- write N/100 values in sequential key order in sync mode
  //   fillseqbatch  -- batch write N values in sequential key order in async mode
  //   fillrandom    -- write N values in random key order in async mode
  //   fillrandsync  -- write N/100 values in random key order in sync mode
  //   fillrandbatch -- batch write N values in sequential key order in async mode
  //   overwrite     -- overwrite N values in random key order in async mode
  //   fillrand100K  -- write N/1000 100K values in random order in async mode
  //   fillseq100K   -- write N/1000 100K values in sequential order in async mode
  //   readseq       -- read N times sequentially
  //   readrandom    -- read N times in random order
  //   readrand100K  -- read N/1000 100K values in sequential order in async mode
  const char *FLAGS_benchmarks =
    "fillseq,"
    "fillseqsync,"
    "fillseqbatch,"
    "fillrandom,"
    "fillrandsync,"
    "fillrandbatch,"
    "overwrite,"
    "overwritebatch,"
    "readrandom,"
    "readseq,"
    "fillrand100K,"
    "fillseq100K,"
    "readseq,"
    "readrand100K,"
    ;
  int FLAGS_num = 100000000;
  int FLAGS_reads = -1;
  int FLAGS_value_size = 100;
  bool FLAGS_histogram = false;
  bool FLAGS_raw = false;
  double FLAGS_compression_ratio = 0.5;
  int FLAGS_page_size = 4096;
  int FLAGS_num_pages = 4096;
  bool FLAGS_use_existing_db = false;
  bool FLAGS_transaction = true;
//  bool FLAGS_WAL_enabled = false;
  char* FLAGS_db = NULL;
  // xxx(homework)
  char FLAGS_journal_mode[100] = "del";
}

void print_usage(const char* argv0) {
  fprintf(stderr, "Usage: %s [OPTION]...\n", argv0);
  fprintf(stderr, "SQLite3 benchmark tool\n");
  fprintf(stderr, "[OPTION]\n");
  fprintf(stderr, "  --benchmarks=[BENCH]\t\tspecify benchmark\n");
  fprintf(stderr, "  --histogram={0,1}\t\trecord histogram\n");
  fprintf(stderr, "  --raw={0,1}\t\t\toutput raw data\n");
  fprintf(stderr, "  --compression_ratio=DOUBLE\tcompression ratio\n");
  fprintf(stderr, "  --use_existing_db={0,1}\tuse existing database\n");
  fprintf(stderr, "  --num=INT\t\t\tnumber of entries\n");
  fprintf(stderr, "  --reads=INT\t\t\tnumber of reads\n");
  fprintf(stderr, "  --value_size=INT\t\tvalue size\n");
  fprintf(stderr, "  --no_transaction\t\tdisable transaction\n");
  fprintf(stderr, "  --page_size=INT\t\tpage size\n");
  fprintf(stderr, "  --num_pages=INT\t\tnumber of pages\n");
//  fprintf(stderr, "  --WAL_enabled={0,1}\t\tenable WAL\n");
  fprintf(stderr, "  --db=PATH\t\t\tpath to location databases are created\n");
  fprintf(stderr, "  --help\t\t\tshow this help\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "[BENCH]\n");
  fprintf(stderr, "  fillseq\twrite N values in sequential key order in async mode\n");
  fprintf(stderr, "  fillseqsync\twrite N/100 values in sequential key order in sync mode\n");
  fprintf(stderr, "  fillseqbatch\tbatch write N values in sequential key order in async mode\n");
  fprintf(stderr, "  fillrandom\twrite N values in random key order in async mode\n");
  fprintf(stderr, "  fillrandsync\twrite N/100 values in random key order in sync mode\n");
  fprintf(stderr, "  fillrandbatch\tbatch write N values in random key order in async mode\n");
  fprintf(stderr, "  overwrite\toverwrite N values in random key order in async mode\n");
  fprintf(stderr, "  fillrand100K\twrite N/1000 100K values in random order in async mode\n");
  fprintf(stderr, "  fillseq100K\twirte N/1000 100K values in sequential order in async mode\n");
  fprintf(stderr, "  readseq\tread N times sequentially\n");
  fprintf(stderr, "  readrandom\tread N times in random order\n");
  fprintf(stderr, "  readrand100K\tread N/1000 100K values in sequential order in async mode\n");

} 


int main (int argc, char** argv) {
  
  init();
  char* default_db_path = static_cast<char*>(malloc(sizeof(char) * 1024));
  strcpy(default_db_path, "./");

  for (int i = 1; i < argc; i++) {
    double d;
    int n;
    char junk;
    char journal_mode[100];

    if (starts_with(argv[i], "--benchmarks=")) {
      FLAGS_benchmarks = argv[i] + strlen("--benchmarks=");
    } else if (sscanf(argv[i], "--histogram=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_histogram = n;
    } else if (sscanf(argv[i], "--raw=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_raw = n;
    } else if (sscanf(argv[i], "--compression_ratio=%lf%c", &d, &junk) == 1) {
      FLAGS_compression_ratio = d;
    } else if (sscanf(argv[i], "--use_existing_db=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_use_existing_db = n;
    } else if (sscanf(argv[i], "--num=%d%c", &n, &junk) == 1) {
      FLAGS_num = n;
    } else if (sscanf(argv[i], "--reads=%d%c", &n, &junk) == 1) {
      FLAGS_reads = n;
    } else if (sscanf(argv[i], "--value_size=%d%c", &n, &junk) == 1) {
      FLAGS_value_size = n;
    } else if (!strcmp(argv[i], "--no_transaction")) {
      FLAGS_transaction = false;
    } else if (sscanf(argv[i], "--page_size=%d%c", &n, &junk) == 1) {
      FLAGS_page_size = n;
    } else if (sscanf(argv[i], "--num_pages=%d%c", &n, &junk) == 1) {
      FLAGS_num_pages = n;
    }
    else if (sscanf(argv[i], "--journal_mode=%s", journal_mode) == 1) {
      strncpy(FLAGS_journal_mode,journal_mode,100);
      fprintf(stderr, "journal_mode = %s\n", journal_mode);

//    }
//    } else if (sscanf(argv[i], "--WAL_enabled=%d%c", &n, &junk) == 1 &&
//               (n == 0 || n == 1)) {
//      FLAGS_WAL_enabled = n;

    } else if (strncmp(argv[i], "--db=", 5) == 0) {
      FLAGS_db = argv[i] + 5;
    } else if (!strcmp(argv[i], "--help")) {
      print_usage(argv[0]);
      exit(0);
    } else {
      fprintf(stderr, "Invalid flag '%s'\n", argv[i]);
      exit(1);
    }
  }

  /* Choose a location for the test database if none given with --db=<path>  */
  if (FLAGS_db == NULL)
      FLAGS_db = default_db_path;

	Benchmark benchmark;

  benchmark.benchmark_init();
  benchmark.benchmark_run();
  benchmark.benchmark_close();

  return 0;
}


//} // namespace sqliteBench
