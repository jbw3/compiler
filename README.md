# Compiler

## Wiki

Check the [Wiki](https://github.com/jbw3/compiler/wiki) for more information on the compiler and the WIP language.

## Getting Started

### Linux

#### Dependencies

LLVM 20 is required to build the compiler.
LLVM's website has [instructions](https://apt.llvm.org) for installing on several popular Linux distributions.
For example, run the following commands to install LLVM 20 on Ubuntu 24.04:

```bash
# add repository to package manager
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
sudo apt-add-repository -y "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-20 main"

# install
sudo apt-get update
sudo apt-get install -y clang-20
```

#### Building

Clone the repo and go to the root directory.

```
git clone git@github.com:jbw3/compiler.git
cd compiler
```

Build with `cmake`.

Debug:
```
mkdir debug
cd debug
cmake ../src -DCMAKE_CXX_COMPILER=clang++-20 -DLLVM_DIR=/usr/lib/llvm-20/cmake -DCMAKE_BUILD_TYPE=Debug
make
```

Release:
```
mkdir release
cd release
cmake ../src -DCMAKE_CXX_COMPILER=clang++-20 -DLLVM_DIR=/usr/lib/llvm-20/cmake -DCMAKE_BUILD_TYPE=Release
make
```

### Windows

#### Dependencies

LLVM 18 is required to build the compiler.
The standard LLVM Windows install is not sufficient to be able to build against LLVM.
Instead, the LLVM source must be downloaded and built from scratch.
Download the source code (Source code (tar.gz)) from [here](https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8).

Configure the LLVM project.
The following assumes the LLVM source has been extracted into a directory named `llvm-project-llvmorg-18.1.8`.
```
mkdir llvm-project-llvmorg-18.1.8\llvm\llvm-debug
cd llvm-project-llvmorg-18.1.8\llvm\llvm-debug
cmake ..\llvm-18.1.8.src -Thost=x64
```

Build and install debug:
```
cmake --build . --config Debug --target INSTALL
```

#### Building

Clone the repo and go to the root directory.

```
git clone git@github.com:jbw3/compiler.git
cd compiler
```

Build with `cmake`.
```
mkdir build
cd build
cmake ..\src -DLLVM_DIR="C:\Program Files (x86)\LLVM\lib\cmake"
cmake --build .
```
