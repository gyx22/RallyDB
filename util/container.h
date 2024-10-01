#include <cstdint>
#include <unordered_map>
#include <vector>

#include "leveldb/export.h"
#include "leveldb/slice.h"
#include "leveldb/table.h"

#include "util/sstable_worth.h"
#include "util/table_object.h"

namespace leveldb {
class Handle;
struct ContainerHandle {
  TableObject* value;
  ContainerHandle* next;
  ContainerHandle* prev;
  size_t charge;  // TODO(opt): Only allow uint32_t?
  uint32_t hash;  // Hash of key(); used for fast sharding and comparisons

  size_t level;
  size_t bf_number;
};

class Container {
 public:
  Container(SSTableWorth* worth);

  ~Container();

  Handle* Insert(TableObject* table, size_t level);

  void Erase(Handle* handle);

  bool LoadBloomFilter(SequenceNumber current_time, uint64_t next_period_time,
                       int64_t* all_size, int64_t* memory, int64_t* charger,
                       std::vector<ContainerHandle*>* loaders,
                       bool* break_flag);

  bool EvictBloomFilter(SequenceNumber current_time, int64_t* all_size,
                        int64_t* memory, int64_t* charger,
                        std::vector<ContainerHandle*>* evicters);

 private:
  friend class MultiContainer;
  ContainerHandle* head_;
  ContainerHandle* tail_;

  void AddToHead(ContainerHandle* handle);
  void RemoveHandle(ContainerHandle* handle);
  void MoveToHead(ContainerHandle* handle);

  SSTableWorth* sstable_worth_;
};
}  // namespace leveldb