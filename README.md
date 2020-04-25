# Compiler

## Getting Started

### Dependencies

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

### Building

Clone the repo and run `make` in the root directory to build the compiler.

```
git clone git@github.com:jbw3/compiler.git
cd compiler
make
```

## Wiki

Check the [Wiki](https://github.com/jbw3/compiler/wiki) for more information on the compiler and the WIP language.
