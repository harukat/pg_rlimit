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
    text      *res_text;
    char      *res_csz;
	int        res;
    struct rlimit rlim;
    int        retval;

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

    if (0 != getrlimit(RLIMIT_AS, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	retval = rlim.rlim_cur;
	if (retval == -1)
		retval = rlim.rlim_max; 

	PG_RETURN_INT32(retval);
}

Datum
pg_setrlimit(PG_FUNCTION_ARGS)
{
    text      *res_text;
    char      *res_csz;
	int        res;
    int32      lim;
    struct rlimit rlim;

    res_text = PG_GETARG_TEXT_P(0);
    res_csz = text_to_cstring(res_text);
    lim = PG_GETARG_INT32(1);

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

    if (0 != setrlimit(RLIMIT_AS, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("setrlimit(2) failed: %m")));

	PG_RETURN_BOOL(1);
}

