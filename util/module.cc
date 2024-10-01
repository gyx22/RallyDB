#include "util/module.h"

#include <iostream>
#include <torch/script.h>  // One-stop header.
#include <torch/torch.h>

namespace leveldb {
Module::Module(const std::string& path) {
  device_ = torch::kCPU;
  if (torch::cuda::is_available()) {
    device_ = torch::kCUDA;
  }

  try {
    module_ = torch::jit::load(path);
    module_.to(device_);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

// 将hist_times里的值都取出来，进行归一化，
// 然后放入torch::jit::script::Module的变量module_里训练
// 将输出的结果返回
float Module::forward(std::deque<float> history_times, uint64_t windows_number) {
  assert(history_times.size() == windows_number);
  torch::Tensor input_tensor = torch::ones({1, (long int)windows_number}, device_);

  for (int i = 0; i < history_times.size(); i++) {
    input_tensor[0][i] = history_times[i];
  }

  // 进行归一化处理
  auto mean = input_tensor.mean();
  auto std = input_tensor.std();
  input_tensor.sub_(mean).div_(std);

  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(input_tensor);

  // 将归一化后的数据传递给 module_ 进行训练
  at::Tensor output = module_.forward(inputs).toTensor();

  output = output.mul_(std).add_(mean);
  output = output.mean();
  return output.item().toFloat();
}
}  // namespace leveldb