#include "util/create_table.h"

#include "util/container.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(CreateTableTest, demo) {
  CreateTable create_table;
  TableObject* t = create_table.GetTableOject();  // free by CreateTable object
  delete t;
  ASSERT_NE(t, nullptr);
}
}  // namespace leveldb