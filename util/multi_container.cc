#include "leveldb/multi_container.h"

/*
db
table cache
multi container
----------------
container
lru
bloom filter
*/

namespace leveldb {
MultiContainer::MultiContainer(SSTableWorth* worth) : charger_(0) {
  multi_container_.resize(kAllFilterUnitsNumber + 1);
  for (int i = 0; i < kAllFilterUnitsNumber + 1; i++) {
    multi_container_[i].resize(config::kNumLevels + 1);
    for (int j = 0; j < config::kNumLevels + 1; j++) {
      multi_container_[i][j] = new Container(worth);
    }
  }
}

MultiContainer::~MultiContainer() {
  for (int i = 0; i < multi_container_.size(); i++) {
    for (int j = 0; j < multi_container_[i].size(); j++) {
      delete multi_container_[i][j];
    }
  }
}

Handle* MultiContainer::Insert(const Slice& key, int level,
                               TableObject* table_object) {
  MutexLock l(&mutex_);
  int bf_number = table_object->table->BFNumer();
  Container* container = multi_container_[bf_number][level];
  Handle* handle = container->Insert(table_object, level);
  hash_tabale_[key.ToString()] = handle;
  charger_ += (table_object->table->BFChange());
  return handle;
}

Handle* MultiContainer::Lookup(const Slice& key) {
  MutexLock l(&mutex_);
  auto iter = hash_tabale_.find(key.ToString());
  if (iter != hash_tabale_.end()) {
    Handle* handle = iter->second;
    ContainerHandle* c_handle = reinterpret_cast<ContainerHandle*>(handle);
    size_t bf_number = c_handle->bf_number;
    size_t level = c_handle->level;
    Container* container = multi_container_[bf_number][level];
    container->MoveToHead(c_handle);

    return reinterpret_cast<Handle*>(handle);
  } else {
    return nullptr;
  }
}

Table* MultiContainer::Value(Handle* handle) {
  if (handle != nullptr) {
    ContainerHandle* container_handle =
        reinterpret_cast<ContainerHandle*>(handle);
    return container_handle->value->table;
  } else {
    return nullptr;
  }
}

void MultiContainer::Erase(const Slice& key) {
  MutexLock l(&mutex_);
  auto iter = hash_tabale_.find(key.ToString());
  if (iter != hash_tabale_.end()) {
    Handle* handle = iter->second;
    ContainerHandle* c_handle = reinterpret_cast<ContainerHandle*>(handle);
    Container* container =
        multi_container_[c_handle->bf_number][c_handle->level];
        // TODO: fix multi thread bug
    //charger_ -= c_handle->value->table->BFChange();
    container->Erase(handle);
    hash_tabale_.erase(key.ToString());
  }
}

size_t MultiContainer::TotalCharge() const {
  MutexLock l(&mutex_);
  return charger_;
};

void MultiContainer::LoadBloomFilter(SequenceNumber current_time,
                                     uint64_t next_period_time,
                                     int64_t memory) {
  MutexLock l(&mutex_);
  int64_t all_size = 0;
  do {
    all_size = 0;
    for (int bf_number = 0; bf_number < kAllFilterUnitsNumber; bf_number++) {
      for (int level = 0; level < config::kNumLevels + 1; level++) {
        Container* container = multi_container_[bf_number][level];
        std::vector<ContainerHandle*> loaders;
        bool break_flag = false;
        bool return_flag = container->LoadBloomFilter(
            current_time, next_period_time, &all_size, &memory, &charger_,
            &loaders, &break_flag);
        RemoveFromContainers(loaders, bf_number);
        AddToContainers(loaders, bf_number + 1);
        if (return_flag) return;

        if (break_flag) {
          break;
        }
      }
    }
  } while (memory > 0 && all_size != 0);
}

void MultiContainer::EvictBloomFilter(SequenceNumber current_time,
                                      int64_t memory) {
  MutexLock l(&mutex_);
  int64_t all_size = 0;
  do {
    all_size = 0;
    for (int bf_number = kAllFilterUnitsNumber; bf_number > 1; bf_number--) {
      for (int level = config::kNumLevels; level > 0; level--) {
        Container* container = multi_container_[bf_number][level];
        std::vector<ContainerHandle*> evicters;
        bool return_flag = container->EvictBloomFilter(
            current_time, &all_size, &memory, &charger_, &evicters);
        RemoveFromContainers(evicters, bf_number);
        AddToContainers(evicters, bf_number - 1);
        if (return_flag) return;
      }
    }
  } while (memory > 0 && all_size != 0);
}

void MultiContainer::AddToContainers(
    const std::vector<ContainerHandle*>& numebrs, int bf_number) {
  if (bf_number < 0 || bf_number > kAllFilterUnitsNumber || numebrs.empty()) {
    return;
  }

  for (int i = 0; i < numebrs.size(); i++) {
    int level = numebrs[i]->level;
    Container* container = multi_container_[level][bf_number];
    ContainerHandle* handle = reinterpret_cast<ContainerHandle*>(numebrs[i]);
    container->AddToHead(handle);
  }
}

void MultiContainer::RemoveFromContainers(
    const std::vector<ContainerHandle*>& numebrs, int bf_number) {
  if (numebrs.empty()) {
    return;
  }

  for (int i = 0; i < numebrs.size(); i++) {
    int level = numebrs[i]->level;
    Container* container = multi_container_[level][bf_number];
    ContainerHandle* handle = reinterpret_cast<ContainerHandle*>(numebrs[i]);
    container->RemoveHandle(handle);
  }
}

MultiContainer* NewMultiContainer(SSTableWorth* worth) {
  return new MultiContainer(worth);
}
};  // namespace leveldb