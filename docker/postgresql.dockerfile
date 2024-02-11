FROM postgres

USER postgres
ENV POSTGRES_PASSWORD libsese

COPY script/postgres_dump.sql /docker-entrypoint-initdb.d/postgres_dump.sql


