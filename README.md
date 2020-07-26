# Compiler

## Wiki

Check the [Wiki](https://github.com/jbw3/compiler/wiki) for more information on the compiler and the WIP language.

## Getting Started

### Linux

#### Dependencies

LLVM 10 is required to build the compiler.
LLVM's website has [instructions](https://apt.llvm.org) for installing on several popular Linux distributions.
For example, run the following commands to install LLVM 10 on Ubuntu 18.04:

```bash
# add repository to package manager
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null | apt-key add -
apt-add-repository "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-10 main"

# install
sudo apt-get update
sudo apt-get install -y clang-10
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
cmake ../src -DCMAKE_CXX_COMPILER=clang++-10 -DLLVM_DIR=/usr/lib/llvm-10/cmake -DCMAKE_BUILD_TYPE=Debug
make
```

Release:
```
mkdir release
cd release
cmake ../src -DCMAKE_CXX_COMPILER=clang++-10 -DLLVM_DIR=/usr/lib/llvm-10/cmake -DCMAKE_BUILD_TYPE=Release
make
```

### Windows

#### Dependencies

LLVM 10 is required to build the compiler.
The standard LLVM Windows install is not sufficient to be able to build against LLVM.
Instead, the LLVM [source](https://releases.llvm.org/download.html) must be downloaded and built from scratch.
CMake version 3.8.0 or higher is needed.
The following assumes the LLVM source has been extracted into a directory named `llvm-10.0.0.src`.

Configure the LLVM project.
```
mkdir llvm-build
cd llvm-build
cmake ..\llvm-10.0.0.src -Thost=x64
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
