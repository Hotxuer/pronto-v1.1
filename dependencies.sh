#!/bin/bash
yum -y install git-core

yum -y install numactl
yum -y install build-essential
yum -y install uuid-dev
yum -y install pkg-config
yum -y install libndctl-dev
yum -y install libdaxctl-dev
yum -y install autoconf
yum -y install cmake
yum -y install python
yum -y install curl
yum -y install libz-dev
yum -y install doxygen
yum -y install graphviz

# curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
curl https://bootstrap.pypa.io/pip/2.7/get-pip.py -o get-pip.py
python get-pip.py
python -m pip install numpy

wget https://github.com/jemalloc/jemalloc/archive/5.1.0.tar.gz && \
    tar -xzvf 5.1.0.tar.gz && rm 5.1.0.tar.gz && \
    cd jemalloc-5.1.0 && ./autogen.sh && ./configure && make -j && \
    make install && cd ..

wget https://github.com/pmem/pmdk/archive/1.4.1.tar.gz && \
    tar -xzvf 1.4.1.tar.gz && rm 1.4.1.tar.gz && cd pmdk-1.4.1 && \
    make -j && make install && cd ..

exit 0
