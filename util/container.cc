#include "util/container.h"

namespace leveldb {
Container::Container(SSTableWorth* worth) : sstable_worth_(worth) {
  head_ = new ContainerHandle();
  tail_ = new ContainerHandle();

  head_->next = tail_;
  head_->prev = nullptr;

  tail_->prev = head_;
  tail_->next = nullptr;
}

Container::~Container() {
  ContainerHandle* curr = head_;
  while (curr) {
    ContainerHandle* tmp = curr->next;
    delete curr;
    curr = tmp;
  }
}

Handle* Container::Insert(TableObject* table_object, size_t level) {
  ContainerHandle* handle = new ContainerHandle();
  handle->next = nullptr;
  handle->prev = nullptr;
  handle->level = level;

  int bf_number = table_object->table->BFNumer();
  handle->bf_number = bf_number;

  handle->value = table_object;
  AddToHead(handle);

  return reinterpret_cast<Handle*>(handle);
}

void Container::Erase(Handle* handle) {
  ContainerHandle* c_handle = reinterpret_cast<ContainerHandle*>(handle);
  RemoveHandle(c_handle);
  delete c_handle;
}

bool Container::LoadBloomFilter(SequenceNumber current_time,
                                uint64_t next_period_time, int64_t* all_size,
                                int64_t* memory, int64_t* charger,
                                std::vector<ContainerHandle*>* loaders,
                                bool* break_flag) {
  *break_flag = false;
  ContainerHandle* curr = head_->next;  // MRU
  while (curr && curr != tail_) {
    if (curr->value->table->ISCold(current_time)) {
      return (*memory <= 0);
    }

    uint64_t table_id = curr->value->table->TableID();
    int bf_number = curr->bf_number;
    int bits_per_key = curr->value->table->BitsPerKey();

    if (sstable_worth_ && !sstable_worth_->IsWorth(table_id, next_period_time,
                                                   bf_number, bits_per_key)) {
      *break_flag = true;
      return (*memory <= 0);
    }

    ContainerHandle* tmp = curr->next;

    size_t size = curr->value->table->UnitSize();
    curr->value->table->LoadBloomFilter();
    *all_size += size;
    *memory -= size;
    *charger += size;

    loaders->push_back(curr);

    if (*memory <= 0) {
      return true;
    }

    curr = tmp;
  }

  return *memory <= 0;
}

bool Container::EvictBloomFilter(SequenceNumber current_time, int64_t* all_size,
                                 int64_t* memory, int64_t* charger,
                                 std::vector<ContainerHandle*>* evicters) {
  ContainerHandle* curr = tail_->prev;  // LRU
  while (curr && curr != head_) {
    ContainerHandle* tmp = curr->prev;

    if (!curr->value->table->ISCold(current_time)) {
      return *memory <= 0;
    }

    size_t size = curr->value->table->UnitSize();
    curr->value->table->EvictBloomFilter();

    *all_size += size;
    *memory -= size;
    *charger -= size;

    evicters->push_back(curr);

    if (*memory <= 0) {
      return true;
    }

    curr = tmp;
  }

  return *memory <= 0;
}

void Container::AddToHead(ContainerHandle* handle) {
  handle->next = head_->next;
  handle->prev = head_;

  head_->next->prev = handle;
  head_->next = handle;
}

void Container::RemoveHandle(ContainerHandle* handle) {
  handle->next->prev = handle->prev;
  handle->prev->next = handle->next;
}

void Container::MoveToHead(ContainerHandle* handle) {
  RemoveHandle(handle);
  AddToHead(handle);
}
}  // namespace leveldb