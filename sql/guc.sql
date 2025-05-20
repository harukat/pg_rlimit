SELECT setting ~ 'pg_rlimit' FROM pg_settings WHERE name = 'shared_preload_libraries';
\echo precondition: shared_preload_libraries = 'pg_rlimit'
SHOW pg_rlimit.v ;
SELECT length(repeat(string_agg(md5(g::text), ','),1024)) FROM generate_series(1, 10000) g;
SET pg_rlimit.v TO '1GB';
SHOW pg_rlimit.v ;
SET pg_rlimit.v TO '100MB';
SHOW pg_rlimit.v ;
SELECT length(repeat(string_agg(md5(g::text), ','),1024)) FROM generate_series(1, 10000) g;
\echo You can ignore the diff of detail messages
SET pg_rlimit.v TO '-1';
SHOW pg_rlimit.v ;
