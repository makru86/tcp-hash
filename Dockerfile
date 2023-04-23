# syntax=docker/dockerfile:1
FROM ubuntu:20.04
RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata
RUN apt-get update \
  && apt-get install -y build-essential \
      gcc \
      g++ \
      gdb \
      clang \
      make \
      ninja-build \
      cmake \
      valgrind \
      locales-all \
      dos2unix \
      rsync \
      tar \
    libboost-all-dev \
    libxxhash-dev \
    googletest         \
    libbenchmark1      \
    libbenchmark-dev   \
    libbenchmark-tools \
    \
    ncat \
    xxhash \
    \
    netcat \
    nmap \
    netcat-openbsd \
    vim \
    \
    clang-format \
    clang-tidy \
    gcovr \
  && apt-get clean
RUN cd /usr/src/googletest && \
    cmake CMakeLists.txt && \
    make install && \
    mv lib/*.a /usr/lib && \
    mv googletest/include/gtest /usr/include && \
    mv googlemock/include/gmock /usr/include
