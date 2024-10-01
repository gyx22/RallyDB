#include "util/lr.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(LRTest, Demo) {
  std::vector<double> a = {1, 2, 3, 4, 5};
  std::vector<double> b = {2, 4, 6, 8, 10};

  LeastSquare l(a, b);
  ASSERT_EQ(l.return_a(), 2);
  ASSERT_EQ(l.return_b(), 0);
}
}  // namespace leveldb