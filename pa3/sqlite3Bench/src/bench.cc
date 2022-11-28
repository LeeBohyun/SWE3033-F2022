#include "bench.h"

namespace sqliteBench {

// Comma-separated list of operations to run in the specified order benchmarks
char* FLAGS_benchmarks;

// Number of key/values to place in database
int FLAGS_num;

// Number of read operations to do.  If negative, do FLAGS_num reads.
int FLAGS_reads;

// Size of each value
int FLAGS_value_size;

// Print histogram of operation timings
bool FLAGS_histogram;

// Print raw data
bool FLAGS_raw;

// Arrange to generate values that shrink to this fraction of
// their original size after compression
double FLAGS_compression_ratio;

// Page size. Default 1 KB.
int FLAGS_page_size;

// Number of pages.
// Default cache size = FLAGS_page_size * FLAGS_num_pages = 4 MB.
int FLAGS_num_pages;

// If true, do not destroy the existing database.  If you set this
// flag and also specify a benchmark that wants a fresh database, that benchmark will fail.
bool FLAGS_use_existing_db;

// If true, we allow batch writes to occur
bool FLAGS_transaction;

// If true, we enable Write-Ahead Logging
//bool FLAGS_WAL_enabled;

// Use the db with the following name.
char* FLAGS_db; 

// xxx(homeworak)
char FLAGS_journal_mode[100];

inline void wal_checkpoint(sqlite3* db_) {
  /* Flush all writes to disk */
  //if (FLAGS_WAL_enabled) {
  //  sqlite3_wal_checkpoint_v2(db_, NULL, SQLITE_CHECKPOINT_FULL, NULL,
  //                            NULL);
  //}
}

void Benchmark::print_header() {
  const int kKeySize = 16;
  print_env();
  fprintf(stderr, "Keys:       %d bytes each\n", kKeySize);
  fprintf(stderr, "Values:     %d bytes each\n", FLAGS_value_size);  
  fprintf(stderr, "Entries:    %d\n", num_);
  fprintf(stderr, "RawSize:    %.1f MB (estimated)\n",
            (((int64_t)(kKeySize + FLAGS_value_size) * num_)
            / 1048576.0));
  print_warnings();
  fprintf(stderr, "------------------------------------------------\n");
}

void Benchmark::print_warnings() {
#if defined(__GNUC__) && !defined(__OPTIMIZE__)
  fprintf(stderr,
      "WARNING: Optimization is disabled: benchmarks unnecessarily slow\n"
      );
#endif
#ifndef NDEBUG
  fprintf(stderr,
      "WARNING: Assertions are enabled: benchmarks unnecessarily slow\n"
      );
#endif
}

void Benchmark::print_env() {
  fprintf(stderr, "SQLite:     version %s\n", SQLITE_VERSION);
#if defined(__linux)
  time_t now = time(NULL);
  fprintf(stderr, "Date:       %s", ctime(&now));

  FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
  if (cpuinfo != NULL) {
    char line[1000];
    int num_cpus = 0;
    char* cpu_type = static_cast<char*>(malloc(sizeof(char) * 1000));
    char* cache_size = static_cast<char*>(malloc(sizeof(char) * 1000));
    while (fgets(line, sizeof(line), cpuinfo) != NULL) {
      char* sep = strchr(line, ':');
      if (sep == NULL) {
        continue;
      }
      char* key = static_cast<char*>(calloc(sizeof(char), 1000));
      char* val = static_cast<char*>(calloc(sizeof(char), 1000));
      strncpy(key, line, sep - 1 - line);
      strcpy(val, sep + 1);
      char* trimed_key = trim_space(key);
      char* trimed_val = trim_space(val);
      free(key);
      free(val);
      if (!strcmp(trimed_key, "model name")) {
        ++num_cpus;
        strcpy(cpu_type, trimed_val);
      } else if (!strcmp(trimed_key, "cache size")) {
        strcpy(cache_size, trimed_val);
      }
      free(trimed_key);
      free(trimed_val);
    }
    fclose(cpuinfo);
    fprintf(stderr, "CPU:        %d * %s\n", num_cpus, cpu_type);
    fprintf(stderr, "CPUCache:   %s\n", cache_size);
    free(cpu_type);
    free(cache_size);
  }
#endif
}

void Benchmark::start() {
  start_ =  now_micros() * 1e-6;
  bytes_ = 0;
  message_ = static_cast<char*>(malloc(sizeof(char) * 10000));
  strcpy(message_, "");
  last_op_finish_ = start_;
  hist_.Clear();
  raw_.raw_clear();
  done_ = 0;
  next_report_ = 100;
}

void Benchmark::finished_single_op() {
  if (FLAGS_histogram || FLAGS_raw) {
    double now = now_micros() * 1e-6;
    double micros = (now - last_op_finish_) * 1e6;
    if (FLAGS_histogram) {
      hist_.Add(micros);
      if (micros > 20000) {
        fprintf(stderr, "long op: %.1f micros%30s\r", micros, "");
        fflush(stderr);
      }
    }
    if (FLAGS_raw) {
      raw_.raw_add(micros);
    }
    last_op_finish_ = now;
  }

  done_++;
  if (done_ >= next_report_) {
    if      (next_report_ < 1000)   next_report_ += 100;
    else if (next_report_ < 5000)   next_report_ += 500;
    else if (next_report_ < 10000)  next_report_ += 1000;
    else if (next_report_ < 50000)  next_report_ += 5000;
    else if (next_report_ < 100000) next_report_ += 10000;
    else if (next_report_ < 500000) next_report_ += 50000;
    else                            next_report_ += 100000;
    fprintf(stderr, "... finished %d ops%30s\r", done_, "");
    fflush(stderr);
  }
}

void Benchmark::stop(const char* name) {
  double finish = now_micros() * 1e-6;

  if (done_ < 1) done_ = 1;

  if (bytes_ > 0) {
    char *rate = static_cast<char*>(malloc(sizeof(char) * 100));
    snprintf(rate, strlen(rate), "%6.1f MB/s",
              (bytes_ / 1048576.0) / (finish - start_));
    if (message_ && !strcmp(message_, "")) {
      message_ = strcat(strcat(rate, " "), message_);
    } else {
      message_ = rate;
    }
  }

  fprintf(stderr, "%-12s : %11.3f micros/op;%s%s\n",
          name,
          (finish - start_) * 1e6 / done_,
          (!message_ || !strcmp(message_, "") ? "" : " "),
          (!message_) ? "" : message_);
  if (FLAGS_raw) {
    raw_.raw_print(stdout);
  }
  if (FLAGS_histogram) {
    fprintf(stderr, "Microseconds per op:\n%s\n",
            hist_.ToString().c_str());
  }
  fflush(stdout);
  fflush(stderr);
}

void Benchmark::benchmark_init() {
  db_ = NULL;
  db_num_ = 0;
  if (FLAGS_num == 0) {
    num_ = 100000;
  } else {
    num_ = FLAGS_num;
  }
  reads_ = FLAGS_reads < 0 ? FLAGS_num : FLAGS_reads;
  bytes_ = 0;
  gen_.rand_gen_init(FLAGS_compression_ratio);
  gen_.rand_init(301);

  struct dirent* ep;
  DIR* test_dir = opendir(FLAGS_db);


  if (!FLAGS_use_existing_db) {
    while ((ep = readdir(test_dir)) != NULL) {
      if (starts_with(ep->d_name, "sqlitebench_db")) {
        char file_name[1000];
        strcpy(file_name, FLAGS_db);
        strcat(file_name, ep->d_name);
        remove(file_name);
      }
    }
  }
  closedir(test_dir);
}

void Benchmark::benchmark_close() {
  int status = sqlite3_close(db_);
  error_check(status);
}

void Benchmark::benchmark_run() {
  print_header();
  benchmark_open();

  char* benchmarks = FLAGS_benchmarks;
  while (benchmarks != NULL) {
    char* sep = strchr(benchmarks, ',');
    char* name;
    if (sep == NULL) {
      name = benchmarks;
      benchmarks = NULL;
    } else {
      name = static_cast<char*>(calloc(sizeof(char), (sep - benchmarks + 1)));
      strncpy(name, benchmarks, sep - benchmarks);
      benchmarks = sep + 1;
    }
    bytes_ = 0;
    start();
    bool known = true;
    bool write_sync = false;
    if (!strcmp(name, "fillseq")) {
      benchmark_write(write_sync, SEQUENTIAL, FRESH, num_, FLAGS_value_size, 1);
      wal_checkpoint(db_);
    } // xxx(homework)
    else if (!strcmp(name, "directfillrandom")) {
      benchmark_directFillRand(num_);      
    } 
    else if (!strcmp(name, "example")) {
      benchmark_example();
    }
    else if (!strcmp(name, "fillrandom")) {
      known=false;
      fprintf(stderr, "not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillseqbatch")) {
      known=false;
      fprintf(stderr, "not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillrandbatch")) {
      known=false;
      fprintf(stderr, "not supported yet '%s'\n", name);
    } else if (!strcmp(name, "overwrite")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "overwritebatch")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillrandsync")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillseqsync")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillrand100K")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "fillseq100K")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "readseq")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "readrandom")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else if (!strcmp(name, "readrand100K")) {
      known=false;
      fprintf(stderr, "Not supported yet '%s'\n", name);
    } else {
      known = false;
      if (strcmp(name, "")) {
        fprintf(stderr, "unknown benchmark '%s'\n", name);
      }
    }
    if (known) {
      stop(name);
    }
  }
}

void Benchmark::benchmark_open() {
  assert(db_ == NULL);

  int status;
  char file_name[100];
  char* err_msg = NULL;
  db_num_++;

  /* Open database */
  char *tmp_dir = FLAGS_db;
  snprintf(file_name, sizeof(file_name),
            "%s/sqlitebench_test-%d.db",
            tmp_dir,
            db_num_);

  // (jhpark): always use fresh db
  if (!FLAGS_use_existing_db) {
    remove(file_name);
  }

  status = sqlite3_open(file_name, &db_);
  if (status) {
    fprintf(stderr, "open error: %s\n", sqlite3_errmsg(db_));
    exit(1);
  }

  /* Change SQLite cache size */
  char cache_size[100];
  snprintf(cache_size, sizeof(cache_size), "PRAGMA cache_size = %d",
            FLAGS_num_pages);
  status = sqlite3_exec(db_, cache_size, NULL, NULL, &err_msg);
  exec_error_check(status, err_msg);

  // xxx(homework): set page size
  benchmark_setPageSize();
  // xxx(homework): set journal mode
  benchmark_setJournalMode();

  /* Change locking mode to exclusive and create tables/index for database */
  const char* locking_stmt = "PRAGMA locking_mode = EXCLUSIVE";
  const char* create_stmt =
          "CREATE TABLE test (key blob, value blob, PRIMARY KEY (key))";
  const char* stmt_array[] = { locking_stmt, create_stmt, NULL };
  int stmt_array_length = sizeof(stmt_array) / sizeof(char*);
  for (int i = 0; i < stmt_array_length; i++) {
    status = sqlite3_exec(db_, stmt_array[i], NULL, NULL, &err_msg);
    exec_error_check(status, err_msg);
  }
}

void Benchmark::benchmark_write(bool write_sync, int order, int state,
                  int num_entries, int value_size, int entries_per_batch) {
  /* Create new database if state == FRESH */
  if (state == FRESH) {
    if (FLAGS_use_existing_db) {
      message_ = static_cast<char*>(malloc(sizeof(char) * 100));
      strcpy(message_, "skipping (--use_existing_db is true)");
      return;
    }
    sqlite3_close(db_);
    db_ = NULL;
    benchmark_open();
    start();
  }

  if (num_entries != num_) {
    char* msg = static_cast<char*>(malloc(sizeof(char) * 100));
    snprintf(msg, 100, "(%d ops)", num_entries);
    message_ = msg;
  }

  char* err_msg = NULL;
  int status;

  sqlite3_stmt *replace_stmt, *begin_trans_stmt, *end_trans_stmt;
  const char* replace_str = "REPLACE INTO test (key, value) VALUES (?, ?)";
  const char* begin_trans_str = "BEGIN TRANSACTION";
  const char* end_trans_str = "END TRANSACTION";

  /* Check for synchronous flag in options */
  const char* sync_stmt = (write_sync) ? "PRAGMA synchronous = FULL" :
                                    "PRAGMA synchronous = OFF";
  status = sqlite3_exec(db_, sync_stmt, NULL, NULL, &err_msg);
  exec_error_check(status, err_msg);

  /* Preparing sqlite3 statements */
  status = sqlite3_prepare_v2(db_, replace_str, -1,
                              &replace_stmt, NULL);
  error_check(status);
  status = sqlite3_prepare_v2(db_, begin_trans_str, -1,
                              &begin_trans_stmt, NULL);
  error_check(status);
  status = sqlite3_prepare_v2(db_, end_trans_str, -1,
                              &end_trans_stmt, NULL);
  error_check(status);

  bool transaction = (entries_per_batch > 1);
  for (int i = 0; i < num_entries; i += entries_per_batch) {
    /* Begin write transaction */
    if (FLAGS_transaction && transaction) {
      status = sqlite3_step(begin_trans_stmt);
      step_error_check(status);
      status = sqlite3_reset(begin_trans_stmt);
      error_check(status);
    }

    /* Create and execute SQL statements */
    for (int j = 0; j < entries_per_batch; j++) {
      const char* value = gen_.rand_gen_generate(value_size);

      /* Create values for key-value pair */
      const int k = (order == SEQUENTIAL) ? i + j :
                    (gen_.rand_next() % num_entries);
      char key[100];
      snprintf(key, sizeof(key), "%016d", k);

      /* Bind KV values into replace_stmt */
      status = sqlite3_bind_blob(replace_stmt, 1, key, 16, SQLITE_STATIC);
      error_check(status);
      status = sqlite3_bind_blob(replace_stmt, 2, value,
                                  value_size, SQLITE_STATIC);
      error_check(status);

      /* Execute replace_stmt */
      bytes_ += value_size + strlen(key);
      status = sqlite3_step(replace_stmt);
      step_error_check(status);

      /* Reset SQLite statement for another use */
      status = sqlite3_clear_bindings(replace_stmt);
      error_check(status);
      status = sqlite3_reset(replace_stmt);
      error_check(status);

      finished_single_op();
    }

    /* End write transaction */
    if (FLAGS_transaction && transaction) {
      status = sqlite3_step(end_trans_stmt);
      step_error_check(status);
      status = sqlite3_reset(end_trans_stmt);
      error_check(status);
    }
  }

  status = sqlite3_finalize(replace_stmt);
  error_check(status);
  status = sqlite3_finalize(begin_trans_stmt);
  error_check(status);
  status = sqlite3_finalize(end_trans_stmt);
  error_check(status);
}

void Benchmark::benchmark_read(int order, int entries_per_batch) {
  int status;
  sqlite3_stmt *read_stmt, *begin_trans_stmt, *end_trans_stmt;

  const char *read_str = "SELECT * FROM test WHERE key = ?";
  const char *begin_trans_str = "BEGIN TRANSACTION";
  const char *end_trans_str = "END TRANSACTION";

  /* Preparing sqlite3 statements */
  status = sqlite3_prepare_v2(db_, begin_trans_str, -1,
                              &begin_trans_stmt, NULL);
  error_check(status);
  status = sqlite3_prepare_v2(db_, end_trans_str, -1,
                              &end_trans_stmt, NULL);
  error_check(status);
  status = sqlite3_prepare_v2(db_, read_str, -1,
                              &read_stmt, NULL);
  error_check(status);

  bool transaction = (entries_per_batch > 1);
  for (int i = 0; i < reads_; i += entries_per_batch) {
    /* Begin read transaction */
    if (FLAGS_transaction && transaction) {
      status = sqlite3_step(begin_trans_stmt);
      step_error_check(status);
      status = sqlite3_reset(begin_trans_stmt);
      error_check(status);
    }

    /* Create and execute SQL statements */
    for (int j = 0; j < entries_per_batch; j++) {
      /* Create key value */
      char key[100];
      int k = (order == SEQUENTIAL) ? i + j : (gen_.rand_next() % reads_);
      snprintf(key, sizeof(key), "%016d", k);

      /* Bind key value into read_stmt */
      status = sqlite3_bind_blob(read_stmt, 1, key, 16, SQLITE_STATIC);
      error_check(status);
      
      /* Execute read statement */
      while ((status = sqlite3_step(read_stmt)) == SQLITE_ROW) {}
      step_error_check(status);

      /* Reset SQLite statement for another use */
      status = sqlite3_clear_bindings(read_stmt);
      error_check(status);
      status = sqlite3_reset(read_stmt);
      error_check(status);
      finished_single_op();
    }

    /* End read transaction */
    if (FLAGS_transaction && transaction) {
      status = sqlite3_step(end_trans_stmt);
      step_error_check(status);
      status = sqlite3_reset(end_trans_stmt);
      error_check(status);
    }
  }

  status = sqlite3_finalize(read_stmt);
  error_check(status);
  status = sqlite3_finalize(begin_trans_stmt);
  error_check(status);
  status = sqlite3_finalize(end_trans_stmt);
  error_check(status);
}

} // namespace sqliteBench
