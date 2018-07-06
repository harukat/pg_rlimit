
CREATE FUNCTION pg_catalog.pg_setrlimit(text, int)
  RETURNS boolean AS 'MODULE_PATHNAME', 'pg_setrlimit'
  LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.pg_getrlimit(text)
  RETURNS int AS 'MODULE_PATHNAME', 'pg_getrlimit'
  LANGUAGE C STRICT;

