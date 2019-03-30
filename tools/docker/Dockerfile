FROM ubuntu:16.04

# install dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    git \
    make \
    software-properties-common \
    wget

# point package manager to clang/LLVM
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null | apt-key add - && \
    apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-8 main"

# install clang/LLVM
RUN apt-get update && apt-get install -y \
    clang-8
