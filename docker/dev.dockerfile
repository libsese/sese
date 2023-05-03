FROM ubuntu

RUN apt-get update -y && \
    apt-get install -y build-essential tar curl zip unzip git pkg-config cmake

RUN git clone https://github.com/libsese/sese.core.git /root/sese.core

WORKDIR /root/sese.core

RUN git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg && \
    /usr/local/vcpkg/bootstrap-vcpkg.sh && \
    /usr/local/vcpkg/vcpkg install