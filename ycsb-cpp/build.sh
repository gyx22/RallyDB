#!/bin/bash

# 获取当前脚本所在的目录
CURRENT_PATH=$(pwd)
COMMON_PATH="${CURRENT_PATH}/../"

# 检查是否从命令行输入了 LIBTORCH_PATH
if [ -z "$1" ]; then
  # 如果没有输入，从 libtorch.txt 文件中读取路径
  LIBTORCH_FILE="${COMMON_PATH}/shell/libtorch.txt"
  if [ -f "${LIBTORCH_FILE}" ]; then
    LIBTORCH_PATH=$(head -n 1 "${LIBTORCH_FILE}")
  else
    echo "Error: libtorch.txt not found and no LIBTORCH_PATH provided."
    exit 1
  fi
else
  LIBTORCH_PATH="$1"
fi

rm -rf build
mkdir build
cd build

cmake -DBIND_LEVELDB=1 \
      -DLEVELDB_INCLUDE_DIR="${COMMON_PATH}/include" \
      -DCMAKE_PREFIX_PATH="${LIBTORCH_PATH}" \
      -DLEVELDB_LIB="${COMMON_PATH}/build/libleveldb.a" \
      ..

make -j