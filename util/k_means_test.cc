#include "util/k_means.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(KMeansTest, Demo) {
  KMEANS<double> kms(7);
  vector<vector<double>> input_data_set;
  for (int i = 0; i < 2000; i++) {
    vector<double> data;
    data.push_back(1 + i);
    data.push_back(2 + i);
    input_data_set.push_back(data);
  }

  kms.loadDataSet(input_data_set);
  kms.randCent();
  kms.kmeans();
}
}  // namespace leveldb