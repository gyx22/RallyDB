// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/table_cache.h"

#include "db/filename.h"
#include "leveldb/env.h"
#include "leveldb/table.h"
#include "util/coding.h"
#include "util/table_cache_lru.h"

namespace leveldb {

struct TableAndFile {
  RandomAccessFile* file;
  Table* table;
};

static void DeleteEntry(const Slice& key, void* value) {
  TableAndFile* tf = reinterpret_cast<TableAndFile*>(value);
  delete tf->table;
  delete tf->file;
  delete tf;
}

static void UnrefEntry(void* arg1, void* arg2) {
  TableCacheLRU* cache = reinterpret_cast<TableCacheLRU*>(arg1);
  TableCacheLRU::Handle* h = reinterpret_cast<TableCacheLRU::Handle*>(arg2);
  cache->Release(h);
}

TableCache::TableCache(const std::string& dbname, const Options& options,
                       int entries, SSTableWorth* worth)
    : env_(options.env),
      dbname_(dbname),
      options_(options),
      cache_(NewTableCacheLRU(entries, worth)) {}

TableCache::~TableCache() { delete cache_; }

Status TableCache::NewRandomAccessFileForTable(const std::string& fname, RandomAccessFile** file){
  if(options_.using_direct_io){
    return env_->NewDirectIORandomAccessFile(fname, file);
  }
  return env_->NewRandomAccessFile(fname, file);
}

Status TableCache::FindTable(uint64_t file_number, uint64_t file_size, int level,
                             TableCacheLRU::Handle** handle) {
  Status s;
  char buf[sizeof(file_number)];
  EncodeFixed64(buf, file_number);
  Slice key(buf, sizeof(buf));
  *handle = cache_->Lookup(key);
  if (*handle == nullptr) {
    std::string fname = TableFileName(dbname_, file_number);
    RandomAccessFile* file = nullptr;
    Table* table = nullptr;
    s = NewRandomAccessFileForTable(fname, &file);
    if (!s.ok()) {
      std::string old_fname = SSTTableFileName(dbname_, file_number);
      if (NewRandomAccessFileForTable(old_fname, &file).ok()) {
        s = Status::OK();
      }
    }
    if (s.ok()) {
      s = Table::Open(options_, file, file_size, &table, file_number);
    }

    if (!s.ok()) {
      assert(table == nullptr);
      delete file;
      // We do not cache error results so that if the error is transient,
      // or somebody repairs the file, we recover automatically.
    } else {
      TableObject* object = new TableObject;
      object->source = file;
      object->table = table;
      *handle = cache_->Insert(key, object, level, 1);
    }
  }
  return s;
}

Iterator* TableCache::NewIterator(const ReadOptions& options,
                                  uint64_t file_number, uint64_t file_size, int level,
                                  Table** tableptr) {
  if (tableptr != nullptr) {
    *tableptr = nullptr;
  }

  TableCacheLRU::Handle* handle = nullptr;
  Status s = FindTable(file_number, file_size, level, &handle);
  if (!s.ok()) {
    return NewErrorIterator(s);
  }

  Table* table = reinterpret_cast<TableObject*>(cache_->Value(handle))->table;
  Iterator* result = table->NewIterator(options);
  result->RegisterCleanup(&UnrefEntry, cache_, handle);
  if (tableptr != nullptr) {
    *tableptr = table;
  }
  return result;
}

Status TableCache::Get(const ReadOptions& options, uint64_t file_number,
                       uint64_t file_size, int level, const Slice& k, void* arg,
                       void (*handle_result)(void*, const Slice&,
                                             const Slice&)) {
  TableCacheLRU::Handle* handle = nullptr;
  Status s = FindTable(file_number, file_size, level, &handle);
  if (s.ok()) {
    Table* t = reinterpret_cast<TableObject*>(cache_->Value(handle))->table;
    s = t->InternalGet(options, k, arg, handle_result);
    cache_->Release(handle);
  }
  return s;
}

void TableCache::Evict(uint64_t file_number) {
  char buf[sizeof(file_number)];
  EncodeFixed64(buf, file_number);
  cache_->Erase(Slice(buf, sizeof(buf)));
}

void TableCache::LoadBloomFilter(SequenceNumber current_time, uint64_t next_period_time, int memory){
  cache_->LoadBloomFilter(current_time, next_period_time,memory);
}

void TableCache::EvictBloomFilter(SequenceNumber current_time, int memory){
  cache_->EvictBloomFilter(current_time, memory);
}

}  // namespace leveldb
