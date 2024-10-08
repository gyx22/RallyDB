// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_TABLE_H_
#define STORAGE_LEVELDB_INCLUDE_TABLE_H_

#include <cstdint>

#include "leveldb/multi_queue.h"
#include "leveldb/export.h"
#include "leveldb/iterator.h"
#include "table/filter_block.h"

namespace leveldb {

class Block;
class BlockHandle;
class Footer;
struct Options;
class RandomAccessFile;
struct ReadOptions;
class TableCache;

// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class LEVELDB_EXPORT Table {
 public:
  // Attempt to open the table that is stored in bytes [0..file_size)
  // of "file", and read the metadata entries necessary to allow
  // retrieving data from the table.
  //
  // If successful, returns ok and sets "*table" to the newly opened
  // table.  The client should delete "*table" when no longer needed.
  // If there was an error while initializing the table, sets "*table"
  // to nullptr and returns a non-ok status.  Does not take ownership of
  // "*source", but the client must ensure that "source" remains live
  // for the duration of the returned table's lifetime.
  //
  // *file must remain live while this Table is in use.
  static Status Open(const Options& options, RandomAccessFile* file,
                     uint64_t file_size, Table** table, uint64_t table_id);

  static Status return_sstable_kv_number(const Options& options, RandomAccessFile* file,
                     uint64_t file_size, int *kv_number);

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;

  ~Table();

  uint64_t LoadBloomFilter();

  uint64_t EvictBloomFilter();

  // Returns a new iterator over the table contents.
  // The result of NewIterator() is initially invalid (caller must
  // call one of the Seek methods on the iterator before using it).
  Iterator* NewIterator(const ReadOptions&) const;

  // Given a key, return an approximate byte offset in the file where
  // the data for that key begins (or would begin if the key were
  // present in the file).  The returned value is in terms of file
  // bytes, and so includes effects like compression of the underlying data.
  // E.g., the approximate offset of the last key in the table will
  // be close to the file length.
  uint64_t ApproximateOffsetOf(const Slice& key) const;

  static std::string ParseHandleKey(const Options& options, uint64_t file_id);
  static std::string ParseHandleKey(const Options* options, uint64_t file_id);

  size_t BFChange() const;

  size_t UnitSize() const;

  size_t BFNumer() const;

  bool ISCold(SequenceNumber current_time) const;

  uint64_t TableID() const;

  uint64_t BitsPerKey() const;
  
 private:
  friend class TableCache;
  struct Rep;

  static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);

  explicit Table(Rep* rep) : rep_(rep) {}

  // Calls (*handle_result)(arg, ...) with the entry found after a call
  // to Seek(key).  May not make such a call if filter policy says
  // that key is not present.
  Status InternalGet(const ReadOptions&, const Slice& key, void* arg,
                     void (*handle_result)(void* arg, const Slice& k,
                                           const Slice& v));

  FilterBlockReader* ReadFilter();
  void ReadMeta();

  Rep* const rep_;

  void ParseHandleKey();

  bool MultiQueueKeyMayMatch(uint64_t block_offset, const Slice& key);
};
}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_TABLE_H_
