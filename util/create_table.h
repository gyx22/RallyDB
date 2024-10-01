#include "leveldb/table.h"

namespace leveldb {
class TableConstructor;
class TableObject;
class MyConstructor;
class CreateTable {
 public:
  CreateTable();

  ~CreateTable();

  TableObject* GetTableOject();

 private:
  Options options_;
  MyConstructor* tc_;
};
}  // namespace leveldb