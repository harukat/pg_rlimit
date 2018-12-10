pg_rlimit
=========

pg_rlimit is an extension that provides interface for setrlimit(2)
and getrlimit(2) by GUC parameters and SQL functions.

This can be used to avoid OS breakdown by invoking OOM killer or
a great many page faults when a SQL that uses much memory runs.

## requirements and limitation

pg_rlimit needs setrlimit(2) and getrlimit(2) for OS systemcall.

pg_rlimit uses ClientAuthentication_hook for the PostgreSQL source.
PostgreSQL 9.3.x to 10.x are supported.

pg_rlimit now supports only RLIMIT_AS (Max address space).

## install

    $ make 
    $ su -c 'make install'
    $ psql -U postgres -d db -c 'CREATE EXTENSION pg_rlimit'

## usage

### usage by functions

    $ ( ulimit -v 310000 ; pg_ctl start )
    $ psql -U dbuser -d db 
    db=> SELECT * FROM pg_getrlimit('v');
     pg_getrlimit
    --------------
        317440000
    (1 row)
     
    db=> SELECT * FROM pg_setrlimit('v', 257000000);
     pg_setrlimit
    --------------
        257000000
    (1 row)
      
    db=> SELECT * FROM pg_setrlimit('v', 117000000);
     pg_setrlimit
    --------------
        117000000
    (1 row)
     
    db=> \d
    ERROR:  out of memory
    DETAIL:  Failed on request of size 24.
    
    db=> \q


### usage by GUC parameter

    $ vi $PGDATA/postgresql.conf
     
        shared_preload_libraries = 'pg_rlimit'
        pg_rlimit.v = '1GB'
     
    $ pg_ctl restart
    $ psql -U dbuser -d db
     
    db=> SHOW pg_rlimit.v;
     pg_rlimit.v
    -------------
     1GB
    (1 row)
          
    db=> SET pg_rlimit.v TO '512MB';
    SET
    db=> SHOW pg_rlimit.v;
     pg_rlimit.v
    -------------
     512MB
    (1 row)
    
    db=> SELECT * FROM pg_getrlimit('v');
     pg_getrlimit
    --------------
        536870912
    (1 行)


