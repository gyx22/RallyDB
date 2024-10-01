#include "util/container.h"

#include "util/create_table.h"

#include "gtest/gtest.h"

namespace leveldb {
TEST(Container, Demo) {
  Container* container = new Container(nullptr);
  CreateTable create_table;
  TableObject* t = create_table.GetTableOject();
  Handle* handle = container->Insert(t, 0);

  int64_t all_size = 0;
  int64_t memory = 1000;
  int64_t charger = 0;
  std::vector<ContainerHandle*> loader;
  bool break_flag = false;
  container->LoadBloomFilter(0, 0, &all_size, &memory, &charger, &loader,
                             &break_flag);
  ASSERT_EQ(loader.size(), 1);

  container->EvictBloomFilter(kLifeTime * 2, &all_size, &memory, &charger,
                              &loader);
  ASSERT_EQ(loader.size(), 2);

  container->Erase(handle);
  container->LoadBloomFilter(0, 0, &all_size, &memory, &charger, &loader,
                             &break_flag);

  ASSERT_EQ(loader.size(), 2);
  delete container;
  delete t;
}

}  // namespace leveldb
