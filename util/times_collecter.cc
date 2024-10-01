#include "util/times_collecter.h"
#include <filesystem>

#include <atomic>  // 添加头文件

// todo change path
std::string project_path = std::filesystem::canonical(__FILE__).parent_path().parent_path();

std::string path = "/torch/model.pt";

namespace leveldb {
TimesCollecter::TimesCollecter(uint64_t wait_second, uint64_t windows_number,
                               DB* db)
    : get_times_(0),
      timer_(nullptr),
      wait_second_(wait_second),
      windows_number_(windows_number),
      db_(db),
      module_(new Module(project_path + path)) {}

TimesCollecter::~TimesCollecter() {
  delete timer_;
  delete module_;
}

static void TimerCaller(void* ptr) {
  TimesCollecter* collector = reinterpret_cast<TimesCollecter*>(ptr);
  collector->TimerRunner();
}

void TimesCollecter::UpdateGetTimes() {
  get_times_.fetch_add(1);
  if (get_times_ == 1 && !timer_) {
    timer_ = new Timer(TimerCaller, this, wait_second_);
  }
}

void TimesCollecter::TimerRunner() {
  // 更新历史数据窗口
  if (history_times_.size() >=
      windows_number_) {  // 窗口大小是3，保存的数据个数比3少，不弹出
    history_times_.pop_front();
  }

  // 保存数据到历史窗口
  history_times_.push_back(get_times_.load());

  // 清除计数器
  get_times_.store(0);

  // 如果还不够推理。就放弃
  if (history_times_.size() < windows_number_) {
    return;
  }

  // 如果够推理，就放入
  // 把历史数据输入到模型，得到一个输出
  float output = module_->forward(history_times_, windows_number_);

  if (history_times_.empty()) {
    return;
  }

  // 根据输出的数据，启动一个线程，加载布隆过滤器
  AdjustBloomFilter(history_times_[history_times_.size() - 1], output);
}

void TimesCollecter::AdjustBloomFilter(uint64_t old_time, uint64_t times) {
  db_->AdjustBloomFilter(old_time, times);
}

void TimesCollecter::PrintHistoryTimes() {
  for (auto iter = history_times_.begin(); iter != history_times_.end();
       iter++) {
    std::cout << *iter << std::endl;
  }
}
}  // namespace leveldb
