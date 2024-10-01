#include "times_collecter.h"

#include <iostream>

#include "leveldb/env.h"

#include "gtest/gtest.h"

using namespace std;

namespace leveldb {
class FakeDB : public DB {
 public:
  explicit FakeDB(const Options& options) : cnt_(0) {}

  ~FakeDB() override = default;

  Status Put(const WriteOptions& o, const Slice& k, const Slice& v) override {
    return DB::Put(o, k, v);
  }

  Status Delete(const WriteOptions& o, const Slice& key) override {
    return DB::Delete(o, key);
  }
  Status Get(const ReadOptions& options, const Slice& key,
             std::string* value) override {
    assert(false);  // Not implemented
    return Status::NotFound(key);
  }

  Iterator* NewIterator(const ReadOptions& options) override { return nullptr; }

  const Snapshot* GetSnapshot() override { return nullptr; }

  void ReleaseSnapshot(const Snapshot* snapshot) override {}

  Status Write(const WriteOptions& options, WriteBatch* batch) override {
    return Status::OK();
  }

  bool GetProperty(const Slice& property, std::string* value) override {
    return false;
  }
  void GetApproximateSizes(const Range* r, int n, uint64_t* sizes) override {}

  void CompactRange(const Slice* start, const Slice* end) override {}

  void AdjustBloomFilter(uint64_t old, uint64_t output) override { cnt_++; }

  int GetCnt() const { return cnt_; }

 private:
  int cnt_;
};

TEST(TIMES_COLLECTER_TEST, demo) {
  GTEST_SKIP() << "just skip from whole db test, only test alonely";
  Options option;
  option.collect_second = 1;
  FakeDB* db = new FakeDB(option);
  // 每隔1秒收集一次
  TimesCollecter* collecter = new TimesCollecter(1, 3, db);
  int loop = 5;
  int t = loop;

  while (t > 0) {
    // 循环5次，每次1秒钟
    // 一次
    collecter->UpdateGetTimes();
    fprintf(stderr, "loop finished! t=%d\n", t);
    Env::Default()->SleepForMicroseconds(1000000);
    t--;
  }

  int time = db->GetCnt();
  // loop time has loop - 2 windows, just like:
  // loop finished! t=5 |
  // loop finished! t=4 | |
  // loop finished! t=3 | | |
  // loop finished! t=2   | |
  // loop finished! t=1     |
  ASSERT_EQ(time, loop - 3);
  delete collecter;
  delete db;
}
}  // namespace leveldb