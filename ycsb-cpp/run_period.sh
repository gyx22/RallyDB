COMMON_PATH=$(pwd)
TXT_PATH="${COMMON_PATH}/../torch/converted_data.txt"
./build/ycsb -run -db leveldb -P workloads/workload_demo -P leveldb/leveldb.properties -p basic.silent=true -p limit.ops=100 -p limit.file=${TXT_PATH} -s