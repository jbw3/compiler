FROM ubuntu:22.04

# prevent packages from prompting for user input
ARG DEBIAN_FRONTEND=noninteractive

# install dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    cmake \
    git \
    make \
    software-properties-common \
    wget

# point package manager to clang/LLVM
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null | apt-key add - && \
    apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main"

# install clang/LLVM
RUN apt-get update && apt-get install -y \
    clang-18
