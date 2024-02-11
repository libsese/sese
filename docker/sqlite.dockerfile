FROM ubuntu:jammy

COPY sqlite_dump.sql /root/sqlite_dump.sql
COPY docker-entrypoint.sh /root/docker-entrypoint.sh

RUN chmod 755 /root/docker-entrypoint.sh

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends sqlite3 && \
    rm -rf /var/lib/apt/lists/*