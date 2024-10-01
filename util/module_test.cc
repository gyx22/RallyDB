
#include "util/module.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(MODULE_TEST, demo) {
  Module module("/home/wangtingzheng_21/system/RallyDB/torch/model.pt");
  std::cout << module.forward({135.0, 148.0, 148.0}, 3) << std::endl;
}
}  // namespace leveldb