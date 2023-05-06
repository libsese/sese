FROM ubuntu:latest

ENV TERM=xterm-256color

RUN export DEBIAN_FRONTEND=noninteractive && \
    bash -c 'yes | unminimize'

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends apt-transport-https ca-certificates build-essential tar curl zip unzip git pkg-config cmake gcc g++ gdb python3  && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg && \
    /usr/local/vcpkg/bootstrap-vcpkg.sh -disableMetrics