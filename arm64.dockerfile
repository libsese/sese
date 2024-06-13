FROM arm64v8/ubuntu:jammy

ENV TERM=xterm-256color
ENV DEBIAN_FRONTEND=noninteractive

COPY ./scripts/install_ubuntu_deps.sh /tmp/install_deps.sh

RUN yes | unminimize && \
    bash ./tmp/install_deps.sh && \
    apt install -y --no-install-recommends mysql-client postgresql-client sqlite3 python3-pip doxygen && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

COPY ./requirements.txt /tmp/requirements.txt

RUN pip install -r /tmp/requirements.txt && \
    pip cache purge