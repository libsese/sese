--
-- PostgreSQL database dump
--

-- Dumped from database version 15.3
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
-- Name: db_test; Type: DATABASE; Schema: -; Owner: postgres
--

CREATE DATABASE db_test WITH OWNER postgres;


ALTER DATABASE db_test OWNER TO postgres;

\connect db_test

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: tb_begin; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_begin (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_begin OWNER TO postgres;

--
-- Name: tb_commit; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_commit (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_commit OWNER TO postgres;

--
-- Name: tb_delete; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_delete (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_delete OWNER TO postgres;

--
-- Name: tb_getInsertId; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_getInsertId" (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public."tb_getInsertId" OWNER TO postgres;

--
-- Name: tb_insert; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_insert (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_insert OWNER TO postgres;

--
-- Name: tb_query; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_query (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_query OWNER TO postgres;

--
-- Name: tb_dateTime; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_dateTime
(
    id   integer NOT NULL,
    time timestamp NOT NULL
);


ALTER TABLE public.tb_dateTime OWNER TO postgres;

--
-- Name: tb_stmt_getTime; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_getTime
(
    id   integer NOT NULL,
    time timestamp NOT NULL
);


ALTER TABLE public.tb_stmt_getTime OWNER TO postgres;

--
-- Name: tb_stmt_setTime; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_setTime
(
    id   integer NOT NULL,
    time timestamp NOT NULL
);


ALTER TABLE public.tb_stmt_setTime OWNER TO postgres;

--
-- Name: tb_rollBack; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_rollBack (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_rollBack OWNER TO postgres;

--
-- Name: tb_stmt_delete; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_delete (
    id integer,
    name character varying(32)
);


ALTER TABLE public.tb_stmt_delete OWNER TO postgres;

--
-- Name: tb_stmt_insert; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_insert (
    id bigint,
    name character varying(32),
    setDouble double precision,
    setFloat real,
    setInteger integer
);


ALTER TABLE public.tb_stmt_insert OWNER TO postgres;

--
-- Name: tb_stmt_query; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_query (
    id integer,
    name character varying(32)
);


ALTER TABLE public.tb_stmt_query OWNER TO postgres;

--
-- Name: tb_stmt_update; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_update (
    id integer,
    name character varying(32)
);


ALTER TABLE public.tb_stmt_update OWNER TO postgres;

--
-- Name: tb_update; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_update (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_update OWNER TO postgres;

--
-- Name: tb_isNull; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_isNull (
    id integer,
    name character varying(32),
    double_null double precision,
    float_null real,
    long_null bigint,
    dateTime_null timestamp
);


ALTER TABLE public.tb_isNull OWNER TO postgres;

--
-- Name: tb_metadata; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_metadata (
    s smallint not null,
    i integer not null,
    l bigint not null,
    b bool not null,
    str varchar(20) not null,
    f float not null,
    bin bytea not null
);

ALTER TABLE public.tb_metadata OWNER TO postgres;

--
-- Data for Name: tb_commit; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_commit VALUES (1, 'foo');
INSERT INTO public.tb_commit VALUES (2, 'bar');


--
-- Data for Name: tb_delete; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_delete VALUES (1, 'foo');
INSERT INTO public.tb_delete VALUES (2, 'bar');


--
-- Data for Name: tb_getInsertId; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public."tb_getInsertId" VALUES (1, 'foo');
INSERT INTO public."tb_getInsertId" VALUES (2, 'bar');


--
-- Data for Name: tb_insert; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_insert VALUES (1, 'foo');
INSERT INTO public.tb_insert VALUES (2, 'bar');


--
-- Data for Name: tb_query; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_query VALUES (1, 'foo');
INSERT INTO public.tb_query VALUES (2, 'bar');

--
-- Data for Name: tb_begin; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_begin VALUES (1, 'foo');
INSERT INTO public.tb_begin VALUES (2, 'bar');

--
-- Data for Name: tb_dateTime; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_dateTime VALUES (1, '2023-03-18 12:30:00');
INSERT INTO public.tb_dateTime VALUES (2, '2023-03-18 12:30:00');

--
-- Data for Name: tb_stmt_getTime; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_getTime VALUES (1, '2023-03-18 12:30:00');
INSERT INTO public.tb_stmt_getTime VALUES (2, '2023-03-18 12:30:00');

--
-- Data for Name: tb_stmt_setTime; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_setTime VALUES (1, '2023-03-18 12:30:00');
INSERT INTO public.tb_stmt_setTime VALUES (2, '2023-03-18 12:30:00');

--
-- Data for Name: tb_rollBack; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_rollBack VALUES (1, 'foo');
INSERT INTO public.tb_rollBack VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_delete; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_delete VALUES (1, 'foo');
INSERT INTO public.tb_stmt_delete VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_insert; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_insert VALUES (1, 'foo', 1, 1, 1);
INSERT INTO public.tb_stmt_insert VALUES (2, 'bar', 2, 2, 2);


--
-- Data for Name: tb_stmt_query; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_query VALUES (1, 'foo');
INSERT INTO public.tb_stmt_query VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_update; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_update VALUES (1, 'foo');
INSERT INTO public.tb_stmt_update VALUES (2, 'bar');


--
-- Data for Name: tb_update; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_update VALUES (1, 'foo');
INSERT INTO public.tb_update VALUES (2, 'bar');

--
-- Data for Name: tb_stmt_insert; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_isNull VALUES (1, 'foo', 1, 1, 1, '2023-03-18 12:30:00');
INSERT INTO public.tb_isNull VALUES (2, 'bar', 2, 2, 2, '2023-03-18 12:30:00');

--
-- Name: tb_getInsertId tb_getInsertId_pk; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_getInsertId"
    ADD CONSTRAINT "tb_getInsertId_pk" PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

