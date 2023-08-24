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

CREATE DATABASE db_test;


ALTER DATABASE db_test OWNER TO postgres;

\connect db_test

-- SET statement_timeout = 0;
-- SET lock_timeout = 0;
-- SET idle_in_transaction_session_timeout = 0;
-- SET client_encoding = 'UTF8';
-- SET standard_conforming_strings = on;
-- SELECT pg_catalog.set_config('search_path', '', false);
-- SET check_function_bodies = false;
-- SET xmloption = content;
-- SET client_min_messages = warning;
-- SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

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
-- Name: tb_rollBack; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_rollBack" (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public."tb_rollBack" OWNER TO postgres;

--
-- Name: tb_stmt_delete; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_delete (
    id integer NOT NULL,
    name character(32) NOT NULL
);


ALTER TABLE public.tb_stmt_delete OWNER TO postgres;

--
-- Name: tb_stmt_insert; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_insert (
    id integer NOT NULL,
    name character(32) NOT NULL
);


ALTER TABLE public.tb_stmt_insert OWNER TO postgres;

--
-- Name: tb_stmt_query; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_query (
    id integer NOT NULL,
    name character varying(32) NOT NULL
);


ALTER TABLE public.tb_stmt_query OWNER TO postgres;

--
-- Name: tb_stmt_update; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_stmt_update (
    id integer NOT NULL,
    name character(32) NOT NULL
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
-- Data for Name: tb_commit; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_commit VALUES (1, 'foo');
INSERT INTO public.tb_commit VALUES (2, 'bar');


--
-- Data for Name: tb_delete; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_delete VALUES (2, 'bar');
INSERT INTO public.tb_delete VALUES (1, 'foo');


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
-- Data for Name: tb_rollBack; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: tb_stmt_delete; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_delete VALUES (2, 'bar                             ');
INSERT INTO public.tb_stmt_delete VALUES (1, 'foo                             ');


--
-- Data for Name: tb_stmt_insert; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_insert VALUES (1, 'foo                             ');
INSERT INTO public.tb_stmt_insert VALUES (2, 'bar                             ');


--
-- Data for Name: tb_stmt_query; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_query VALUES (1, 'foo');
INSERT INTO public.tb_stmt_query VALUES (2, 'bar');


--
-- Data for Name: tb_stmt_update; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_stmt_update VALUES (2, 'bar                             ');
INSERT INTO public.tb_stmt_update VALUES (1, 'foo                             ');


--
-- Data for Name: tb_update; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.tb_update VALUES (2, 'bar');
INSERT INTO public.tb_update VALUES (1, 'foo');


--
-- Name: tb_getInsertId tb_getInsertId_pk; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_getInsertId"
    ADD CONSTRAINT "tb_getInsertId_pk" PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

