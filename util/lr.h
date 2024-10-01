#pragma once

#include <iostream>
#include <vector>
using namespace std;

namespace leveldb {
class LeastSquare {
  double a, b;

 public:
  LeastSquare(const vector<double>& x, const vector<double>& y);

  double return_a() const { return a; }

  double return_b() const { return b; }
};
}  // namespace leveldb