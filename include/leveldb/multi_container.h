#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "leveldb/export.h"
#include "leveldb/slice.h"
#include "leveldb/table.h"

#include "port/port.h"
#include "util/container.h"

namespace leveldb {
struct Handle {};

class MultiContainer {
 public:
  MultiContainer(SSTableWorth* worth);

  MultiContainer(const MultiContainer&) = delete;
  MultiContainer& operator=(const MultiContainer&) = delete;

  ~MultiContainer();

  Handle* Insert(const Slice& key, int level, TableObject* table);

  Handle* Lookup(const Slice& key);

  Table* Value(Handle* handle);

  void Erase(const Slice& key);

  size_t TotalCharge() const;

  void LoadBloomFilter(SequenceNumber current_time, uint64_t next_period_time,
                       int64_t memory);

  void EvictBloomFilter(SequenceNumber current_time, int64_t memory);

 private:
  std::vector<std::vector<Container*>> multi_container_;
  std::unordered_map<std::string, Handle*> hash_tabale_;
  int64_t charger_;

  void AddToContainers(const std::vector<ContainerHandle*>& numbers,
                       int bf_number);
  void RemoveFromContainers(const std::vector<ContainerHandle*>& numbers,
                            int bf_number);

  mutable port::Mutex mutex_;
};

MultiContainer* NewMultiContainer(SSTableWorth* worth);
};  // namespace leveldb