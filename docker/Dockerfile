FROM ubuntu:22.04


RUN apt-get update -y; apt install software-properties-common -y
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y; apt update -y
RUN apt-get install -y git curl zip unzip tar pkg-config gcc-13 g++-13 build-essential linux-headers-$(uname -r) make autoconf automake
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
RUN update-alternatives --set c++ /usr/bin/g++
RUN update-alternatives --set cc /usr/bin/gcc
