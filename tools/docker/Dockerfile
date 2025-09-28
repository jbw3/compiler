FROM ubuntu:24.04

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
RUN wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc && \
    apt-add-repository -y "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-20 main"

# install clang/LLVM
RUN apt-get update && apt-get install -y \
    clang-20
