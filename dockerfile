FROM ubuntu:jammy

ENV TERM=xterm-256color

COPY ./scripts/install_ubuntu_deps.sh /tmp/install_deps.sh
COPY ./requirements.txt /tmp/requirements.txt

# RUN export DEBIAN_FRONTEND=noninteractive && \
#     bash -c 'yes | unminimize'

RUN apt-get update -y && \
    apt-get install python3-pip docker.io docker-compose -y

RUN pip install -r /tmp/requirements.txt

RUN bash ./tmp/install_deps.sh && \
    rm -rf /var/lib/apt/lists/* \
