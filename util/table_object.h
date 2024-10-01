#include "leveldb/table.h"
#include "leveldb/env.h"

namespace leveldb{
    struct TableObject{
        Table* table;
        RandomAccessFile* source;

        ~TableObject(){
            delete table;
            delete source;
        }
    };
}