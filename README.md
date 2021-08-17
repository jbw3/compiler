# Compiler

## Wiki

Check the [Wiki](https://github.com/jbw3/compiler/wiki) for more information on the compiler and the WIP language.

## Getting Started

### Linux

#### Dependencies

LLVM 12 is required to build the compiler.
LLVM's website has [instructions](https://apt.llvm.org) for installing on several popular Linux distributions.
For example, run the following commands to install LLVM 12 on Ubuntu 20.04:

```bash
# add repository to package manager
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null | apt-key add -
apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-12 main"

# install
sudo apt-get update
sudo apt-get install -y clang-12
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
cmake ../src -DCMAKE_CXX_COMPILER=clang++-12 -DLLVM_DIR=/usr/lib/llvm-12/cmake -DCMAKE_BUILD_TYPE=Debug
make
```

Release:
```
mkdir release
cd release
cmake ../src -DCMAKE_CXX_COMPILER=clang++-12 -DLLVM_DIR=/usr/lib/llvm-12/cmake -DCMAKE_BUILD_TYPE=Release
make
```

### Windows

#### Dependencies

LLVM 12 is required to build the compiler.
The standard LLVM Windows install is not sufficient to be able to build against LLVM.
Instead, the LLVM source must be downloaded and built from scratch.
Download `llvm-12.0.1.src.tar.xz` from [here](https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1).

Configure the LLVM project.
The following assumes the LLVM source has been extracted into a directory named `llvm-12.0.1.src`.
```
mkdir llvm-build
cd llvm-build
cmake ..\llvm-12.0.1.src -Thost=x64
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
