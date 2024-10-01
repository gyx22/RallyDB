#pragma one

#include <atomic>  // 添加头文件
#include <chrono>
#include <condition_variable>  // std::condition_variable
#include <future>
#include <iostream>
#include <thread>

class Timer {
 public:
  // caller: 定时器背后要运行的函数
  // p: caller函数的参数
  // sec: 每隔多少秒执行一次定时任务
  Timer(void (*caller)(void* ptr), void* p, uint64_t sec);

  // 会在当前的时间间隔结束后，结束caller的运行
  ~Timer();

 private:
  std::thread* t_;
};
