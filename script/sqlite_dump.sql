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
COMMIT;