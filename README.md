# RallyDB

run this command to clone whole project and sub module

```shell
git clone --recurse-submodules https://github.com/gyx22/RallyDB.git
```

you should download libtorch, and put it on your server, more detail for download just see [libtorch_install.md](./libtorch_install.md)

create a file named `libtorch.txt`, and put your libtorch path in it, just like:

```
/home/gyx22/libtorch-cuda-11.8-cxx11-abi
```

## LevelDB

if you want to build leveldb, just run `build.sh` shell, this shell will create a
folder caller build, and run cmake and make command to link libtorch and build leveldb

if you want to run unit test and micr-bench, you can run `test.sh`
 
## YCSB

cd to folder `ycsb-cpp`, to run ycsb, you can run `build.sh` to linked leveldb, ycsb-cpp and libtorch
this shell will gen a build folder under ycsb-cpp

run `load.sh` will load database date, the path of the file define in `leveldb/leveldb.properties`
workload define in `workloads`

run `run.sh` will gen a workload hit leveldb, `run_period.sh` will gen a limited workload, more detail can be saw in `run_readme.md`

## Model Trainer

you can find `new_main.py` in model folder, run it will read dataset for dataset folder, and use multi model to trained

## Dataset converter

this tool can convert dataset to ycsb-cpp file, just run `convert_dataset.py` under `torch` folder
