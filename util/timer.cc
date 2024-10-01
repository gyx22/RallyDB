#include "timer.h"

std::atomic<bool> should_stop(false);  // 添加一个原子布尔类型的标志
std::atomic<bool> while_stopped(false);

std::mutex mtx;
std::condition_variable cv;

void task(void (*caller)(void* pa), void* p, uint64_t sec) {
  std::chrono::seconds interval(sec);
  while (!should_stop) {
    std::this_thread::sleep_for(interval);
    caller(p);
  }

  while_stopped = true;
  cv.notify_all();
}

Timer::Timer(void (*caller)(void* ptr), void* p, uint64_t sec) {
  t_ = new std::thread(task, caller, p, sec);
  t_->detach();
}

Timer::~Timer() {
  should_stop = true;
  while (!while_stopped) {
    std::unique_lock<std::mutex> lock(mtx, std::adopt_lock);
    cv.wait(lock);
  }
  delete t_;
}