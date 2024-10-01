#!/bin/bash

# 获取当前脚本所在的目录
COMMON_PATH=$(pwd)

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

# 打印使用的 LIBTORCH_PATH 以供确认
echo "Using LIBTORCH_PATH: ${LIBTORCH_PATH}"

# 清理并创建新的 build 目录
rm -rf build
mkdir build && cd build

# 配置和编译项目
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="${LIBTORCH_PATH}" ../
make -j
