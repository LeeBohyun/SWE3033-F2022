#include "bench.h"

namespace sqliteBench {
  
  // #1. Write a code for setting the journal mode in the SQLite database engine
  // [Requirement]
  // (1) This function set jounral mode using "FLAGS_journal_mode" extern variable (user input)
  // (2) This function check journal mode; if user try to set wrong journal mode then return -2
  // (3) This function returns status (int data type) of sqlite API function
  int Benchmark::benchmark_setJournalMode() {

    // please fill this function
    return 0;

  }

  // #2. Write a code for setting page size in the SQLite database engine
  // [Requirement]
  // (1) This function set page size using "FLAGS_page_size" extern variable (user input)
  // (2) This function return status (int data type) of sqltie API function
  // (3) This function is called at benchmark_open() in bench.cc
  int Benchmark::benchmark_setPageSize() {

    // please fill this function
    return 0;

  }

  // #3. Write a code for insert function (direct SQL execution mode)
  // [Requriement]
  // (1) This function fill random key-value data using direct qurey execution sqlite API function
  //     (please refer to lecture slide project 3)
  // (2) This function has single arguemnt num_ which is total number of operations
  // (3) This function create SQL statement (key-value pair) do not modfiy this code 
  // (4) This function execute given SQL statement
  // (5) This function is called at benchmark_open() in bench.cc
  int Benchmark::benchmark_directFillRand(int num_) {
    //      DO NOT MODIFY HERE     //
    const char* value = gen_.rand_gen_generate(FLAGS_value_size);
    char key[100];
    const int k = gen_.rand_next() % num_;

    snprintf(key, sizeof(key), "%016d", k);
    char fill_stmt[100];
    snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , x'%x')", key ,value);
    ////////////////////////////////

    // execute SQL statement
    // please fill this function
    return 0;
  }

  // xxx(homework)
  // write your own benchmark functions
  // you can add multiple functions as you like 
  // you can change function name. Here example is literally example.
  int Benchmark::benchmark_example() {
    fprintf(stderr, "example functions works!\n");
    return 0;
  }

}; // namespace sqliteBench

