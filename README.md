pg_rlimit
=========

pg_rlimit is an extension that provides interfaces for setrlimit(2)
and getrlimit(2) as SQL function.

This can be used to avoid OS breakdown by invoking OOM killer or
a great many page faults when a SQL that uses much memory runs.


## install

    $ make USE_PGXS=1
    $ make USE_PGXS=1 install
    $ psql -U postgres -d db -c 'CREATE EXTENSION pg_rlimit'

## usage

    $ ( ulimit -v 310000 ; pg_ctl start )
     
    db1=# SELECT * FROM pg_getrlimit('v');
     pg_getrlimit
    --------------
        317440000
    (1 row)
     
    db1=# SELECT * FROM pg_setrlimit('v', 257000000);
     pg_setrlimit
    --------------
     t
    (1 row)
      
    db1=# SELECT * FROM pg_setrlimit('v', 117000000);
     pg_setrlimit
    --------------
     t
    (1 row)
     
    db1=# \d
    ERROR:  out of memory
    LINE 12: ORDER BY 1,2;
                      ^
    DETAIL:  Failed on request of size 16224.


