FROM mariadb

ENV MARIADB_USER libsese
ENV MARIADB_PASSWORD libsese
ENV MARIADB_ROOT_PASSWORD libsese

ENV TERM=xterm-256color

RUN export DEBIAN_FRONTEND=noninteractive && \
    bash -c 'yes | unminimize'

COPY script/mysql_dump.sql /docker-entrypoint-initdb.d/mysql_dump.sql
COPY script/sqlite_dump.sql /root/sqlite_dump.sql

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends apt-transport-https ca-certificates build-essential tar curl zip unzip git pkg-config cmake gcc g++ gdb python3 sqlite3 && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/microsoft/vcpkg.git /usr/local/vcpkg && \
    /usr/local/vcpkg/bootstrap-vcpkg.sh -disableMetrics

RUN sqlite3 /root/db_test.db < /root/sqlite_dump.sql