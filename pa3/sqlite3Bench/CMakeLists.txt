
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
