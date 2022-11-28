# SWE3033-F2022
SKKU Database Project Fall 2022

This course is mainly based on the last year's course, thanks to the contribution of Dr. An and Dr. Park.

- Instructor: Prof. Sang-Won Lee (swlee@skku.edu)
- TA
    - Jonghyeok Park (akindo19@skku.edu)
    - Bo-Hyun Lee (lia323@skku.edu)

## Introduction

Almost every important computer application must manage, update, and query databases using a database system(s) that best fits its workload. In this course, you will learn the three most popular database systems and their major workloads:

- MySQL and TPC-C (*OLTP workload*)
- RocksDB and YCSB (*Key-value store workload*)
- SQLite and AndroBench (*Mobile workload*)

From these three projects, you can learn how each database system works and understand the characteristics of popular workloads. You can also learn how to measure key (performance-related) metrics and analyze their impacts on performance.

## Schedule

| Week # | Topic              | Contents    | TA     |
| :----: | :----------------- |:----------- | :----: |
| 1      | | [Introduction](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-1) | Bo-Hyun Lee |
| 2      | TPC-C on MySQL  | [Load & run TPC-C, measure tpmC, and key metrics and analyze the results](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-2) | Bo-Hyun Lee |
| 3      | TPC-C on MySQL  |[Introduction to buffer manager, exercise some buffer manager related parameter tuning](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-3)| Bo-Hyun Lee |
| 4      | TPC-C on MySQL |[Buffer manager code review](https://github.com/LeeBohyun/mysql-tpcc/blob/master/buffer_manager/buffer_miss_scenario_monitoring.md), and [project assignment](https://github.com/LeeBohyun/mysql-tpcc/blob/master/buffer_manager/MySQL_project.md)| Bo-Hyun Lee |
| 5      | TPC-C on MySQL |Project #1 MySQL | Bo-Hyun Lee |
| 6      | RocksDB |[Introduction to RocksDB and DBBench](https://github.com/LeeBohyun/SWE3033-F2021/blob/main/week-6/README.md) | Bo-Hyun Lee |
| 7      | RocksDB | DBMS Space Management | Bo-Hyun Lee |
| 8      | | **Mid-term exam** |  |
| 9      | RocksDB | Space Amplification in DBMS | Bo-Hyun Lee |
| 10     | RocksDB |[Project #2 RocksDB](https://github.com/LeeBohyun/SWE3033-F2022/blob/main/pa2.md) | Bo-Hyun Lee |
| 11     | Androbench on SQLite | [Introduction to SQLite and AndroBench](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-10)| Jonghyeok Park |
| 12     | Androbench on SQLite | [Transaction Management in SQlite](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-12) | Jonghyeok Park |
| 13     | Androbench on SQLite | [Logging & Recovery in SQLite](https://github.com/meeeejin/SWE3033-F2021/tree/main/week-14) | Jonghyeok Park |
| 14     | Androbench on SQLite | [Project #3 SQLite](https://github.com/LeeBohyun/SWE3033-F2022/blob/main/pa3/README.md) | Jonghyeok Park |
| 15     | | **Final exam** |  |

## Course Overview
- Lecture video and experiment guide uploaded every Monday: recommend you to watch the videos before Wednesday
- Offline Q&A lab every Wednesday: Ask questions and try to end the experiments during that time
- Submit 2-page report before Sunday

## Contact

- If you have any questions about this course, please contact the teaching assistant: Bo-Hyun Lee(lia323@skku.edu) and Jong-Hyeok Park (akindo19@skku.edu).
- You must attend Q & A session every Wednesday 18:00PM - 19:00PM at 제2과학관 32동 32255.

## References
- https://github.com/meeeejin/SWE3033-F2021/

