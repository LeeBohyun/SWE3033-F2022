# Project 3

## Overview

In this project, you will implement a SQLite benchmark program (SQLiteBench) using C/C++ SQLite API

Follow the guide below. If you have any questions, Please feel free to contact me via email (Jonghyeok Park / akindo19@skku.edu)

> NOTE: This lab is based on the Linux environment. If you don't have a Linux machine, use [VirturalBox](https://www.virtualbox.org/). (Recommend Ubuntu 18.04)

## Instructions

### Prerequisite

- git
```
sudo apt-get install git
```
- cmake
```
sudo apt-get install cmake 
```
- sqlite library
  1. go to the directory you have insatlled SQLite database in week 10.
  2. type `sudo make install -j` command (install library)

```
cd {PATH}/sqlite-src-3360000/build
make -j
sudo make install -j
```

### 0. Download SQLiteBench soruce code

```
git clone https://github.com/meeeejin/SWE3033-F2021.git
cd SWE3033-F2022/pa3

// if you already cloned the reposity then 
cd SWE3033-F2022
git pull

```

### 1. How to compile the `SQLiteBench`

```
cd SWE3033-F2021/week-11/sqlite3Bench
mkdir build && cd build
CC=gcc CXX=g++ cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

### 2. How to run the `SQLiteBench`

```
./sqliteBench --journal_mode=off --benchmarks=directfillrandom
```

### 3. About SQLiteBench 

1. Input user option 
- `SQLiteBench` stores option given by the user in Benchmark in `FLAGS_* `variables

```c++
// main.cc
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
    }
    ... 
}
```

2. Run Benchmark 

- Open database 

```c++
// bench.h
class Benchmark {

 public:
  sqlite3* db_;
  int num_;
  // ellipsis

  void setJournalMode();
  void benchmark_open();

};

// bench.cc 
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
```

- Run specific benchmark operation and measure the time for each operation

```c++
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
    
    // measure the time elapsed.
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
    ...

    if (known) {
      stop(name);
    }
  }
```


3. Show Benchmark results

  ```
  vldb@NVDIMM:~/SWE3033/cmake-project-template/build$ ./sqliteBench --journal_mode=wal --page_size=16384 --benchmarks=directfillrandom
  journal_mode = wal
  SQLite:     version 3.36.0
  Date:       Sun Nov  7 22:50:27 2021
  CPU:        16 * Intel(R) Xeon(R) CPU E5-2640 v3 @ 2.60GHz
  CPUCache:   20480 KB
  Keys:       16 bytes each
  Values:     0 bytes each
  Entries:    100000
  RawSize:    1.5 MB (estimated)
  ------------------------------------------------
  directfillrandom :      69.857 micros/op;
  ```


### 4. How to use SQLite API in SQLiteBench 

1. Link SQLite API in Cmake build configuration (CMakeList.txt)
```C
project(sqlite_bench)

cmake_minimum_required(VERSION 3.19)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

GET_FILENAME_COMPONENT(PROPATH src ABSOLUTE)
message(STATUS "source code dir: ${PROPATH}")

include_directories(include)
FILE(GLOB_RECURSE SRC_FILES ${PROPATH}/*.cc)
add_library(sqliteBenchLib ${SRC_FILES})
add_executable(sqliteBench main.cc)
target_link_libraries(sqliteBench sqliteBenchLib sqlite3)
```


2.  Set `WAL` jounral mode in SQLite databae engine using `sqlite3_exec()` function

> [Warning] `setWalJournalMode()` function is sample for explanation, there is no `setWalJournalMode()` function in `SQLiteBench` program.

```c++
// sample code 
void Benchmark::setWalJournalMode() {
  int status;
  const char* WAL_stmt = "PRAGMA journal_mode = WAL";
  status = sqlite3_exec(db_, WAL_stmt, NULL, NULL, &err_msg);
}

```

### 5. Write Code (homewrok.cc)

1. `benchmark_setJournalMode()`

- Write a code for setting the journal mode in the SQLite database engine
- [Requirement]
  - This function set jounral mode using "FLAGS_journal_mode" extern variable (user input)
  - This function checks journal mode; if user try to set wrong journal mode then return -2
  - This function returns status (int data type) of sqlite API function
 
    ```c++
      int Benchmark::benchmark_setJournalMode() {
        // fill this function
        return 0;
      }
    ```

2. `benchmark_setPageSize()`

- Write a code for setting page size in the SQLite database engine
- [Requirement]
  - This function sets page size using "FLAGS_page_size" extern variable (user input)
  - This function returns status (int data type) of sqltie API function
  - This function is called at benchmark_open() in bench.cc

     ```c++
     int Benchmark::benchmark_setPageSize() {
      // please fill this function
      return 0;
    }
    ```


3. `benchmark_directFillRand(num_)` 

  - Write a code for insert function (direct SQL execution mode)
  - [Requriement]
    - This function fills random key-value data using direct qurey execution sqlite API function
      (please refer to lecture slide week 11)
    - This function has single arguemnt num_ which is total number of operations
    - This function creates SQL statement (key-value pair) do not modfiy this code 
    - This function executes given SQL statement
    - This function is called at benchmark_open() in bench.cc

     ```c++
    int Benchmark::benchmark_directFillRand(int num_) {
      //      DO NOT MODIFY HERE     //
      const char* value = gen_.rand_gen_generate(FLAGS_value_size);
      char key[100];
      const int k = gen_.rand_next() % num_;

      snprintf(key, sizeof(key), "%016d", k); 
      char fill_stmt[100];
      snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , %s)", key ,value);
      ////////////////////////////////

      // execute SQL statement
      // please fill this function

      return 0;
    }
    ```

### Add your own benchmark functions

1. Add your own benchmark function in `sqlite3Bench`. Belows are some examples:
  - Support different workloads (e.g., skewed workload, sequential workload, etc)
  - Support benchmark statistics (e.g., total # of executions, CPU and disk info, etc)

2. Guideline for adding functions (Please see `benchmark_example()` function).

- `SWE3033-F2022/pa3/sqlite3Bench/include/bench.h`

```
class Benchmark {

  // xxx(homework)
  int benchmark_setJournalMode();
  int benchmark_setPageSize();
  int benchmark_directFillRand(int num_);
  int benchmark_example();
 
 };
```

- `SWE3033-F2022/pa3/sqlite3Bench/src/bench.cc`

```
void Benchmark::benchmark_run() {
  if (!strcmp(name, "fillseq")) {
    benchmark_write(write_sync, SEQUENTIAL, FRESH, num_, FLAGS_value_size, 1);
    wal_checkpoint(db_);
  } 
  // xxx(homework)
  else if (!strcmp(name, "directfillrandom")) {
    benchmark_directFillRand(num_);
  }
  else if (!strcmp(name, "example")) {
    benchmark_example();
  } 
}
```

- `SWE3033-F2022/pa3/sqlite3Bench/src/homework.cc`

```
  int Benchmark::benchmark_example() {
    fprintf(stderr, "example functions works!\n");
    return 0;
  }
```


### Report Submission

1. Follow the guideline (Insturuction #5) and write source code (`SWE3033-F2021/week-11/sqlite3Bench/src/homework.cc`) 
2. Write a simple comment (if any, less then 5 sentences) for your implementation in the source code and attach a screenshot of running the command below.
3. Present PPT slides including belows
  - Motivation (i.e., why we buid this benchmark, )
  - Design of `sqlite3Bench` (i.e., Describe overall architecture of `sqlite3Bench`)
  - Implementation (i.e., Explain your code)
  - Demonstration video (i.e., Excute benchmark functions in homework.cc including your own benchmark function)
  - Conclusion


- command
  ```
  ./sqliteBench --journal_mode=wal --page_size=16384 --benchmarks=directfillrandom
  ```
- comment (example)
  - I used xxx API function for yyy and I used zzz to check the journal name

Organize the source code file (only [SWE3033-F2021/week-11/sqlite3Bench/src/homework.cc](https://github.com/LeeBohyun/SWE3033-F2022/blob/main/pa3/sqlite3Bench/src/homework.cc) file)and screenshot into the single zip file. 
  - Record your presentation [2] using your own slide and convert to mp4[3] (Please record your face too)
  - Presentation file name format: {Student ID}-{Name}.mp4 
  - Zip filename format: (e.g., 2021000000-JonghyeokPark-pa3.zip)


### Tip. How to build debug mode in CMake build configuration (Optional)

```
cd SWE3033-F2021/week-11/sqlite3Bench
mkdir build && cd debug_build
CC=gcc CXX=g++ cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Reference
- [1] [SQLite PRAGMA](https://www.sqlite.org/pragma.html)
- [2] [Record PPT slide](https://support.microsoft.com/en-us/office/record-a-presentation-2570dff5-f81c-40bc-b404-e04e95ffab33)
- [3] [Convert PPT Slide to MP4 file](https://support.microsoft.com/en-us/office/save-a-presentation-as-a-movie-file-or-mp4-4e1ebcc1-f46b-47b6-922a-bac76c4a5691)
