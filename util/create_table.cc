#include "create_table.h"
#include <map>
#include "util/table_object.h"
#include "file_impl.h"

namespace leveldb {
// An STL comparator that uses a Comparator
namespace {
struct STLLessThan {
  const Comparator* cmp;

  STLLessThan() : cmp(BytewiseComparator()) {}
  STLLessThan(const Comparator* c) : cmp(c) {}
  bool operator()(const std::string& a, const std::string& b) const {
    return cmp->Compare(Slice(a), Slice(b)) < 0;
  }
};
}  // namespace
typedef std::map<std::string, std::string, STLLessThan> KVMap;
// Helper class for tests to unify the interface between
// BlockBuilder/TableBuilder and Block/Table.
class CreateTableConstructor {
 public:
  explicit CreateTableConstructor(const Comparator* cmp)
      : data_(STLLessThan(cmp)) {}
  virtual ~CreateTableConstructor() = default;

  void Add(const std::string& key, const Slice& value) {
    data_[key] = value.ToString();
  }

  // Finish constructing the data structure with all the keys that have
  // been added so far.  Returns the keys in sorted order in "*keys"
  // and stores the key/value pairs in "*kvmap"
  void Finish(const Options& options, std::vector<std::string>* keys,
              KVMap* kvmap) {
    *kvmap = data_;
    keys->clear();
    for (const auto& kvp : data_) {
      keys->push_back(kvp.first);
    }
    data_.clear();
    Status s = FinishImpl(options, *kvmap);
    assert(s.ok());
  }

  // Construct the data structure from the data in "data"
  virtual Status FinishImpl(const Options& options, const KVMap& data) = 0;

  const KVMap& data() const { return data_; }

  virtual DB* db() const { return nullptr; }  // Overridden in DBConstructor

 private:
  KVMap data_;
};

class MyConstructor : public CreateTableConstructor {
 public:
  MyConstructor(const Comparator* cmp)
      : CreateTableConstructor(cmp), source_(nullptr), table_(nullptr) {}
  ~MyConstructor() override {}
  Status FinishImpl(const Options& options, const KVMap& data) override {
    Reset();
    StringSink sink;
    TableBuilder builder(options, &sink);

    for (const auto& kvp : data) {
      builder.Add(kvp.first, kvp.second);
    }
    Status s = builder.Finish();

    assert(sink.contents().size() == builder.FileSize());

    // Open the table
    source_ = new StringSource(sink.contents());
    Options table_options;
    table_options.comparator = options.comparator;
    table_options.filter_policy = options.filter_policy;
    // no need to ReadMeta
    return Table::Open(table_options, source_, sink.contents().size(), &table_,
                       0);  // ony one table in memory, file id is no means
  }

  TableObject* GetTableObject() const {
    TableObject* object = new TableObject();
    object->table = table_;
    object->source = source_;
    return object;
  }

 private:
  void Reset() {
    // delete table_;
    delete source_;
    table_ = nullptr;
    source_ = nullptr;
  }

  StringSource* source_;
  Table* table_;

  MyConstructor();
};

CreateTable::CreateTable() {
  tc_ = new MyConstructor(BytewiseComparator());
  CreateTableConstructor* constructor =
      reinterpret_cast<CreateTableConstructor*>(tc_);
  for (int i = 0; i < 100; i++) {
    constructor->Add("key" + std::to_string(i), "value");
  }

  std::vector<std::string> keys;
  KVMap kvmap;

  options_.filter_policy = NewBloomFilterPolicy(4);
  constructor->Finish(options_, &keys, &kvmap);
}

CreateTable::~CreateTable() {
  delete options_.filter_policy;
  delete tc_;
}

TableObject* CreateTable::GetTableOject() {
  TableObject* table = tc_->GetTableObject();
  return table;
}
}  // namespace leveldb
