#include "leveldb/multi_container.h"

#include <vector>

#include "leveldb/table.h"

#include "util/create_table.h"

#include "file_impl.h"
#include "gtest/gtest.h"

namespace leveldb {
TEST(MultiContainer, Demo) {
  MultiContainer container(nullptr);
  CreateTable ct;

  // check lookup
  TableObject* to = ct.GetTableOject();
  Handle* handle = container.Insert("key1", 0, to);
  ASSERT_GE(container.TotalCharge(), 0);
  Handle* get_hanle = container.Lookup("key1");
  ASSERT_EQ(handle, get_hanle);

  // check seek
  Table* object = container.Value(get_hanle);
  Iterator* iter = object->NewIterator(ReadOptions());
  iter->Seek("key1");
  ASSERT_TRUE(iter->Valid());
  ASSERT_EQ(iter->value().ToString(), "value");
  delete iter;

  // check load
  size_t old_size = container.TotalCharge();
  container.LoadBloomFilter(0, 0, 1000);
  size_t new_size = container.TotalCharge();
  ASSERT_GE(new_size, old_size);

  // check erase
  container.Erase("key1");
  get_hanle = container.Lookup("key1");
  // ASSERT_EQ(container.TotalCharge(), 0);
  ASSERT_EQ(get_hanle, nullptr);

  delete to;
}
}  // namespace leveldb
