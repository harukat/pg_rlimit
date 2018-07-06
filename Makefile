MODULES = pg_rlimit

PG_CPPFLAGS = -I$(libpq_srcdir) -L$(libdir)
EXTENSION = pg_rlimit
DATA = pg_rlimit--1.0.sql pg_rlimit.control

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


