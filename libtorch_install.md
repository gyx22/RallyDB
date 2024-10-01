1. download libtorch from this [web](https://pytorch.org/get-started/locally/)
    a. Stable-linux-libtorch-c++/java-cuda 11.8
    b. Download here (cxx11 ABI)
2. build with cmake according to this [web](https://pytorch.org/cppdocs/installing.html)
    a. must be release version
    b. debug version will cause abi bug
3. change LIBTORCH_PATH in test.sh and build.sh
4. run test.sh/build.sh