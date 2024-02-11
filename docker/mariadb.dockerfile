FROM mariadb

ENV MARIADB_USER libsese
ENV MARIADB_PASSWORD libsese
ENV MARIADB_ROOT_PASSWORD libsese

COPY script/mysql_dump.sql /docker-entrypoint-initdb.d/mysql_dump.sql