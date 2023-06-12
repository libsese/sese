--
-- PostgreSQL database dump
--

-- Dumped from database version 15.3 (Debian 15.3-1.pgdg110+1)
-- Dumped by pg_dump version 15.3

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: db_test; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE DATABASE db_test;

ALTER DATABASE db_test OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

\c db_test;

--
-- Name: tb_delete; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_delete (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_delete OWNER TO postgres;

--
-- Name: tb_insert; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_insert (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_insert OWNER TO postgres;

--
-- Name: tb_query; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_query (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_query OWNER TO postgres;

--
-- Name: tb_stmt_delete; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_stmt_delete (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_stmt_delete OWNER TO postgres;

--
-- Name: tb_stmt_insert; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_stmt_insert (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_stmt_insert OWNER TO postgres;

--
-- Name: tb_stmt_query; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_stmt_query (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_stmt_query OWNER TO postgres;

--
-- Name: tb_stmt_update; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_stmt_update (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_stmt_update OWNER TO postgres;

--
-- Name: tb_update; Type: TABLE; Schema: db_test; Owner: postgres
--

CREATE TABLE tb_update (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE tb_update OWNER TO postgres;

--
-- Data for Name: tb_delete; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_delete VALUES (1, 'foo');
INSERT INTO tb_delete VALUES (2, 'bar');


--
-- Data for Name: tb_insert; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_insert VALUES (1, 'foo');
INSERT INTO tb_insert VALUES (2, 'bar');


--
-- Data for Name: tb_query; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_query VALUES (1, 'foo');
INSERT INTO tb_query VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_delete; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_stmt_delete VALUES (1, 'foo');
INSERT INTO tb_stmt_delete VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_insert; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_stmt_insert VALUES (1, 'foo');
INSERT INTO tb_stmt_insert VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_query; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_stmt_query VALUES (1, 'foo');
INSERT INTO tb_stmt_query VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_update; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_stmt_update VALUES (1, 'foo');
INSERT INTO tb_stmt_update VALUES (2, 'bar');


--
-- Data for Name: tb_update; Type: TABLE DATA; Schema: db_test; Owner: postgres
--

INSERT INTO tb_update VALUES (1, 'foo');
INSERT INTO tb_update VALUES (2, 'bar');


--
-- PostgreSQL database dump complete
--

