#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_getrlimit);
PG_FUNCTION_INFO_V1(pg_setrlimit);

Datum pg_getrlimit(PG_FUNCTION_ARGS);
Datum pg_setrlimit(PG_FUNCTION_ARGS);

//extern char *text_to_cstring(const text *t);

Datum
pg_getrlimit(PG_FUNCTION_ARGS)
{
    text  *res_text;
    char  *res_csz;
	int    res;
    struct rlimit rlim;
    int64  retval;

    res_text = PG_GETARG_TEXT_P(0);
    res_csz = text_to_cstring(res_text);

	switch (res_csz[0])
	{
		case 'v':
			res = RLIMIT_AS;
			if (res_csz[1] == '\0')
				break;
		default:
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("specified resource is not supported")));
	}

    if (0 != getrlimit(res, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	retval = rlim.rlim_cur;
	if (retval == RLIM_INFINITY)
		retval = rlim.rlim_max; 

	elog(DEBUG5, "retval = %lld", retval);

	PG_RETURN_INT64(retval);
}


Datum
pg_setrlimit(PG_FUNCTION_ARGS)
{
    text      *res_text;
    char      *res_csz;
	int        res;
    uint64 lim;
    int64 retval;
    struct rlimit rlim;

    res_text = PG_GETARG_TEXT_P(0);
    res_csz = text_to_cstring(res_text);
    lim = PG_GETARG_INT64(1);

	elog(DEBUG5, "lim = %llu", lim);

	switch (res_csz[0])
	{
		case 'v':
			res = RLIMIT_AS;
			if (res_csz[1] == '\0')
				break;
		default:
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("specified resource is not supported")));
	}

    if (0 != getrlimit(res, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	rlim.rlim_cur = lim;

    if (0 != setrlimit(res, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("setrlimit(2) failed: %m")));

    if (0 != getrlimit(res, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	retval = rlim.rlim_cur;
	if (retval == RLIM_INFINITY)
		retval = rlim.rlim_max; 

	elog(DEBUG5, "retval = %lld", retval);

	PG_RETURN_INT64(retval);
}

