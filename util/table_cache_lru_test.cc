
#include "util/table_cache_lru.h"

#include <utility>

#include "leveldb/slice.h"
#include "leveldb/status.h"

#include "util/create_table.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(TableCacheLRUTest, demo) {
  TableCacheLRU* lru = NewTableCacheLRU(10, nullptr);
  CreateTable ct;
  TableObject* to = ct.GetTableOject();
  TableCacheLRU::Handle* handle1 = lru->Insert("key1", to, 0, 1);
  lru->Release(handle1);

  TableCacheLRU::Handle* handle2 = lru->Lookup("key1");

  Table* t = reinterpret_cast<TableObject*>(lru->Value(handle2))->table;
  Iterator* iter = t->NewIterator(ReadOptions());
  iter->Seek("key1");
  ASSERT_TRUE(iter->Valid());
  ASSERT_EQ(iter->value().ToString(), "value");
  ASSERT_EQ(handle1, handle2);
  lru->Release(handle1);

  lru->LoadBloomFilter(0, 0, 1000);
  lru->EvictBloomFilter(kLifeTime * 2, 1000);

  ASSERT_EQ(lru->TotalCharge(), 1);
  lru->Erase("key1");
  ASSERT_EQ(lru->TotalCharge(), 0);

  delete lru;
  delete iter;
}
}  // namespace leveldb