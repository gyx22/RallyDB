#ifndef STORAGE_LEVELDB_DB_TIMES_COLLECTER_H_
#define STORAGE_LEVELDB_DB_TIMES_COLLECTER_H_

#include <atomic>  // 添加头文件
#include <chrono>
#include <deque>
#include <future>
#include <iostream>
#include <stdint.h>

#include "leveldb/db.h"

#include "util/module.h"
#include "util/timer.h"

namespace leveldb {
class TimesCollecter {
 public:
  TimesCollecter(uint64_t wait_second, uint64_t windows_number, DB* db);

  void PrintHistoryTimes();

  // 更新
  void UpdateGetTimes();

  void TimerRunner();

  ~TimesCollecter();

 private:
  std::atomic<uint64_t> get_times_;
  std::deque<float> history_times_;
  Module* module_;
  void AdjustBloomFilter(uint64_t old_time, uint64_t access_time);

  Timer* timer_;
  uint64_t wait_second_;
  uint64_t windows_number_;

  DB* db_;
};
}  // namespace leveldb

#endif