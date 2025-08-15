FROM ubuntu:24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        git \
        gdb \
        cmake build-essential \
        libzstd-dev libedit-dev \
        clangd

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        ca-certificates curl gnupg \
    && (curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /etc/apt/keyrings/llvm.gpg) \
    && (echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/llvm.gpg] http://apt.llvm.org/jammy/ llvm-toolchain-jammy main" | tee /etc/apt/sources.list.d/llvm.list > /dev/null ) \
    && apt-get update && apt-get -y install --no-install-recommends \
         clang llvm-dev libclang-dev