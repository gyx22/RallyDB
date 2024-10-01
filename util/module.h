#ifndef STORAGE_LEVELDB_DB_MODULE_H_
#define STORAGE_LEVELDB_DB_MODULE_H_

#include <deque>
#include <iostream>
#include <string>
#include <torch/script.h>  // One-stop header.
#include <torch/torch.h>

namespace leveldb {
class Module {
 public:
  Module(const std::string& path);
  float forward(std::deque<float> history_times, uint64_t windows_number);

 private:
  torch::Device device_ = torch::kCPU;
  torch::jit::script::Module module_;
};
}  // namespace leveldb

#endif