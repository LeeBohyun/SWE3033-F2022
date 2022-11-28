#ifndef BENCH_H_
#define BENCH_H_

#include <iostream>
#include <cstring>
#include <cassert>
#include <dirent.h>
#include <sqlite3.h>

#include "random.h"
#include "histogram.h"
#include "memory_allocator.h"

namespace sqliteBench {

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
extern char* FLAGS_benchmarks;

// Number of key/values to place in database
extern int FLAGS_num;

// Number of read operations to do.  If negative, do FLAGS_num reads.
extern int FLAGS_reads;

// Size of each value
extern int FLAGS_value_size;

// Print histogram of operation timings
extern bool FLAGS_histogram;

// Print raw data
extern bool FLAGS_raw;

// Arrange to generate values that shrink to this fraction of
// their original size after compression
extern double FLAGS_compression_ratio;

// Number of pages.
// Default cache size = FLAGS_page_size * FLAGS_num_pages = 4 MB.
extern int FLAGS_num_pages;

// If true, do not destroy the existing database.  If you set this
// flag and also specify a benchmark that wants a fresh database, that
// benchmark will fail.
extern bool FLAGS_use_existing_db;

// If true, we allow batch writes to occur
extern bool FLAGS_transaction;

// If true, we enable Write-Ahead Logging
//extern bool flags_wal_enabled;

// Use the db with the following name.
extern char* FLAGS_db;

// xxx(homework)
// Journal mode
extern char FLAGS_journal_mode[100];

// Page size. Default 1 KB.
extern int FLAGS_page_size;


class Benchmark {

 public:
  sqlite3* db_;
  int db_num_;
  int num_;
  int reads_;
  double start_;
  double last_op_finish_;
  int64_t bytes_;
  char* message_;
  Histogram hist_;
  Memory_allocator raw_;
  RandomGenerator gen_;
  Random rand_;

 public:
  enum Order {
      SEQUENTIAL,
        RANDOM
  };

  enum DBState {
      FRESH,
        EXISTING
  };

  // state kept for process messages 
  int done_;
  int next_report_;

  // SQLite error check functions
  inline static void exec_error_check(int status, char* err_msg) {
     if (status != SQLITE_OK) {
      std::cerr << "SQL step function error: status : " << err_msg << std::endl;
      exit(1);
     }
  }

  inline static void step_error_check(int status) {
     if (status != SQLITE_OK) {
      std::cerr << "SQL step function error: status : " << status << std::endl;
      exit(1);
     }
  }

	inline static void error_check(int status) {
  	if (status != SQLITE_OK) {
    	std::cerr << "sqlite3 error: status: " << status << std::endl;
    	exit(1);
  	}
	}
	////////////////////////////////////

	// benchmark view
	void print_header();
	void print_warnings();
	void print_env();

	// benchmark core functions
	void start();
	void finished_single_op();
	void stop(const char *name);
	void benchmark_init();
	void benchmark_open();
	void benchmark_close();
	void benchmark_run();

	// benchmark function
	void benchmark_write(bool write_sync, int order, int state,
							int num_entries, int value_size, int entries_per_batch);

	void benchmark_read(int order, int entries_per_batch);	

  // xxx(homework)
  int benchmark_setJournalMode();
  int benchmark_setPageSize();
  int benchmark_directFillRand(int num_);
  int benchmark_example();

};

uint64_t now_micros(void);
bool starts_with(const char*, const char*);
char* trim_space(const char*);


}; // namespace 
#endif




