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
    \
    ncat \
    xxhash \
    \
    netcat \
    nmap \
    netcat-openbsd \
  && apt-get clean