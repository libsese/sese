FROM amd64/alpine:3.20.1

RUN apk add samurai ninja-build autoconf automake git cmake llvm17 clang17 clang17-extra-tools compiler-rt

RUN apk add libtool python3 py3-pip

RUN apk add doxygen mariadb-client postgresql-client

RUN apk add build-base cmake ninja zip unzip curl git

ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN git clone https://github.com/microsoft/vcpkg /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg

COPY ./requirements.txt /tmp/requirements.txt

RUN pip install -r /tmp/requirements.txt --break-system-packages && \
    pip cache purge
