FROM ubuntu:16.04

# Basic packages.
RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  libboost-test-dev \
  && rm -rf /var/lib/apt/lists/*

# Install packages necessary for compilation.
RUN apt-get update && apt-get install -y \
  qt5-default \
  && rm -rf /var/lib/apt/lists/*

COPY . /emuballs

CMD cd /emuballs; /bin/sh
