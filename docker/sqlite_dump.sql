PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE tb_delete
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_delete VALUES(1,'foo');
INSERT INTO tb_delete VALUES(2,'bar');
CREATE TABLE tb_insert
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_insert VALUES(1,'foo');
INSERT INTO tb_insert VALUES(2,'bar');
CREATE TABLE tb_stmt_delete
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_stmt_delete VALUES(1,'foo');
INSERT INTO tb_stmt_delete VALUES(2,'bar');
CREATE TABLE tb_stmt_insert
(
    id   integer,
    name varchar(32)
, setDouble double, setFloat float, setLong bigint, setNull integer);
INSERT INTO tb_stmt_insert VALUES(1,'foo',1.0,1.0,1,1);
INSERT INTO tb_stmt_insert VALUES(2,'bar',2.0,2.0,2,2);
CREATE TABLE tb_stmt_update
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_stmt_update VALUES(1,'foo');
INSERT INTO tb_stmt_update VALUES(2,'bar');
CREATE TABLE tb_update
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_update VALUES(1,'foo');
INSERT INTO tb_update VALUES(2,'bar');
CREATE TABLE tb_commit
(
    id   integer     not null,
    name varchar(32) not null
);
INSERT INTO tb_commit VALUES(1,'foo');
INSERT INTO tb_commit VALUES(2,'bar');
CREATE TABLE tb_rollBack
(
    id   integer     not null,
    name varchar(32) not null
);
INSERT INTO tb_rollBack VALUES(1,'foo');
INSERT INTO tb_rollBack VALUES(2,'bar');
CREATE TABLE IF NOT EXISTS "tb_getInsertId"
(
    id   integer     not null
        constraint tb_getInsertId_pk
            primary key autoincrement,
    name varchar(32) not null
);
INSERT INTO tb_getInsertId VALUES(1,'foo');
INSERT INTO tb_getInsertId VALUES(2,'bar');
CREATE TABLE tb_begin
(
    id   integer     not null,
    name varchar(32) not null
);
INSERT INTO tb_begin VALUES(1,'foo');
INSERT INTO tb_begin VALUES(2,'bar');
CREATE TABLE tb_dateTime
(
    id   integer not null,
    time text    not null
);
INSERT INTO tb_dateTime VALUES(1,'2023-03-18 12:30:00');
INSERT INTO tb_dateTime VALUES(2,'2023-03-18 12:30:00');
CREATE TABLE IF NOT EXISTS "tb_query"
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_query VALUES(1,'foo');
INSERT INTO tb_query VALUES(2,'bar');
CREATE TABLE IF NOT EXISTS "tb_stmt_getTime"
(
    id   integer not null,
    time text    not null
);
INSERT INTO tb_stmt_getTime VALUES(1,'2023-03-18 12:30:00');
INSERT INTO tb_stmt_getTime VALUES(2,'2023-03-18 12:30:00');
CREATE TABLE IF NOT EXISTS "tb_stmt_query"
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_stmt_query VALUES(1,'foo');
INSERT INTO tb_stmt_query VALUES(2,'bar');
CREATE TABLE tb_stmt_setTime
(
    id       integer not null,
    intTime  int,
    charTime text
);
CREATE TABLE IF NOT EXISTS "tb_isNull"
(
    id       integer,
    name     varchar(32),
    double_null   double,
    float_null    float,
    long_null     bigint,
    dateTime_null text
);
INSERT INTO tb_isNull VALUES(1,NULL,NULL,NULL,NULL,NULL);
INSERT INTO tb_isNull VALUES(NULL,'bar',NULL,NULL,NULL,NULL);
DELETE FROM sqlite_sequence;
INSERT INTO sqlite_sequence VALUES('tb_getInsertId',3);
COMMIT;
