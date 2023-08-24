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
CREATE TABLE tb_query
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_query VALUES(1,'foo');
INSERT INTO tb_query VALUES(2,'bar');
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
);
INSERT INTO tb_stmt_insert VALUES(1,'foo');
INSERT INTO tb_stmt_insert VALUES(2,'bar');
CREATE TABLE tb_stmt_query
(
    id   integer,
    name varchar(32)
);
INSERT INTO tb_stmt_query VALUES(1,'foo');
INSERT INTO tb_stmt_query VALUES(2,'bar');
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
DELETE FROM sqlite_sequence;
INSERT INTO sqlite_sequence VALUES('tb_getInsertId',2);
COMMIT;
