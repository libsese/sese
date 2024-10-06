FROM arm64v8/ubuntu:jammy

ENV TERM=xterm-256color
ENV DEBIAN_FRONTEND=noninteractive

#COPY ./scripts/install_ubuntu_deps.sh /tmp/install_deps.sh

RUN yes | unminimize && \
#    bash ./tmp/install_deps.sh && \
    apt install -y --no-install-recommends curl zip unzip tar git mysql-client postgresql-client sqlite3 python3-pip doxygen && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg

COPY ./requirements.txt /tmp/requirements.txt

RUN pip install -r /tmp/requirements.txt && \
    pip cache purge