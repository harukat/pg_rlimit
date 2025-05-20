pg\_rlimit
=========

pg\_rlimit is an extension that provides interface for setrlimit(2)
and getrlimit(2) by GUC parameters and SQL functions.

Both ulimit -v and systemd LimitAS apply to all PostgreSQL processes,
but this extension allows them to be applied on a per-session basis.

## requirements and limitation

pg\_rlimit needs setrlimit(2) and getrlimit(2) for OS systemcall.

pg\_rlimit uses ClientAuthentication\_hook for the PostgreSQL source.
PostgreSQL 9.3.x to 18.x are supported.

pg\_rlimit now supports only RLIMIT\_AS (Max address space).

## install

    $ make 
    $ su -c 'make install'
    $ psql -U postgres -d db -c 'CREATE EXTENSION pg_rlimit'

## usage

### usage by functions

    $ ( ulimit -v 310000 ; pg_ctl start ) ## by kB
    $ psql -U dbuser -d db 
    db=> SELECT * FROM pg_getrlimit('v'); -- by bytes
     pg_getrlimit
    --------------
        317440000
    (1 row)
     
    db=> SELECT * FROM pg_setrlimit('v', 257000000); -- by bytes
     pg_setrlimit
    --------------
        257000000
    (1 row)
      
    db=> SELECT * FROM pg_setrlimit('v', 117000000);
     pg_setrlimit
    --------------
        117000000
    (1 row)
     
    db=> SELECT length(repeat(string_agg(md5(g::text), ','),1024)) FROM generate_series(1, 10000) g;
    ERROR:  out of memory
    DETAIL:  Failed on request of size 337918980 in memory context "ExprContext".

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
    (1 row)


### Note

"Max address space" includes the size of the shared buffer in use.
It is not possible to limit only the use of private memory by backend processes.

