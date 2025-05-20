SELECT * FROM pg_setrlimit('v', -1);
SELECT * FROM pg_getrlimit('v');
SELECT * FROM pg_setrlimit('v', 512 * 1024 * 1024);
SELECT * FROM pg_getrlimit('v');
SELECT length(repeat(string_agg(md5(g::text), ','),1024)) FROM generate_series(1, 10000) g;
\echo You can ignore the diff of detail messages
