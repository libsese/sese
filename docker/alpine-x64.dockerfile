FROM amd64/alpine:3.20.1

RUN apk add samurai autoconf automake git cmake gcc compiler-rt pkgconfig linux-headers bison flex tzdata-utils

RUN apk add libtool python3 py3-pip bash

RUN apk add doxygen mariadb-client postgresql-client

RUN apk add build-base cmake ninja zip unzip curl git

ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN git config --global http.version HTTP/1.1 && \
    git clone https://github.com/microsoft/vcpkg /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg

COPY ./requirements.txt /tmp/requirements.txt

RUN pip install -r /tmp/requirements.txt --break-system-packages && \
    pip cache purge
