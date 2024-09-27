#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <limits.h>

#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/guc.h"
#include "libpq/auth.h"
#include "miscadmin.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_getrlimit);
PG_FUNCTION_INFO_V1(pg_setrlimit);

Datum pg_getrlimit(PG_FUNCTION_ARGS);
Datum pg_setrlimit(PG_FUNCTION_ARGS);

void _PG_init(void);
void _PG_fini(void);
void ClientAuthentication_hook_impl(Port *port, int status);
void value_v_assign(int newval, void *extra);

static ClientAuthentication_hook_type prev_ClientAuthentication_hook = NULL;
static int pg_rlimit_value_v = -1;

void
value_v_assign(int newval, void *extra)
{
	struct rlimit rlim;
	int64  lim = newval;

	elog(DEBUG1, "pg_rlimit value_v_assign: %d (kB)", newval);

	if (0 != getrlimit(RLIMIT_AS, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	if (lim < 0)
		rlim.rlim_cur = RLIM_INFINITY;
	else
		rlim.rlim_cur = (lim << 10 );

	if (0 != setrlimit(RLIMIT_AS, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("setrlimit(2) failed: %m")));
}

void
ClientAuthentication_hook_impl(Port *port, int status)
{
	struct rlimit rlim;
	int	res;
	int64  lim;

	if (-1 != pg_rlimit_value_v)
	{
		res = RLIMIT_AS;
		lim = pg_rlimit_value_v;
		if (0 != getrlimit(res, &rlim))
			return;
		rlim.rlim_cur = ( lim << 10 );
		setrlimit(res, &rlim);
	}

	if (prev_ClientAuthentication_hook)
		prev_ClientAuthentication_hook(port, status);
}


void
_PG_init(void)
{
	if (! process_shared_preload_libraries_in_progress)
		return;

	prev_ClientAuthentication_hook = ClientAuthentication_hook;
	ClientAuthentication_hook = ClientAuthentication_hook_impl;

	DefineCustomIntVariable("pg_rlimit.v",
		"Sets initial rlimit -v (kB)", NULL, &pg_rlimit_value_v,
		-1, -1, INT_MAX, PGC_USERSET, GUC_UNIT_KB, NULL, value_v_assign, NULL);

	elog(DEBUG1, "pg_rlimit loaded");
}

void
_PG_fini(void)
{
	ClientAuthentication_hook = prev_ClientAuthentication_hook;
}


Datum
pg_getrlimit(PG_FUNCTION_ARGS)
{
	text  *res_text;
	char  *res_csz;
	int	res;
	struct rlimit rlim;
	int64  retval;

	res_text = PG_GETARG_TEXT_P(0);
	res_csz = text_to_cstring(res_text);

	if (res_csz[1] != '\0')
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("specified resource is not supported")));

	switch (res_csz[0])
	{
		case 'v':
			res = RLIMIT_AS;
			break;
		default:
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("specified resource \"%s\" is not supported", res_csz)));
	}

	if (0 != getrlimit(res, &rlim))
		ereport(ERROR,
			(errcode(ERRCODE_SYSTEM_ERROR),
			errmsg("getrlimit(2) failed: %m")));

	if (rlim.rlim_cur == RLIM_INFINITY)
		if (rlim.rlim_max == RLIM_INFINITY)
			retval = -1LL;
		else
			retval = rlim.rlim_max; 
	else
		retval = rlim.rlim_cur;

	elog(DEBUG5, "pg_setrlimit cur:%llu max:%llu return:%lld",
		(long long unsigned int) rlim.rlim_cur,
		(long long unsigned int) rlim.rlim_max, (long long int) retval);

	PG_RETURN_INT64(retval);
}


Datum
pg_setrlimit(PG_FUNCTION_ARGS)
{
	text	  *res_text;
	char	  *res_csz;
	int		res;
	int64 lim;
	int64 retval;
	struct rlimit rlim;

	res_text = PG_GETARG_TEXT_P(0);
	res_csz = text_to_cstring(res_text);
	lim = PG_GETARG_INT64(1);

	elog(DEBUG5, "pg_setrlimit(%lld) called", (long long int) lim);

	if (res_csz[1] != '\0')
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("specified resource is not supported")));

	switch (res_csz[0])
	{
		case 'v':
			res = RLIMIT_AS;
			break;
		default:
			ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("specified resource \"%s\" is not supported", res_csz)));
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

	if (rlim.rlim_cur == RLIM_INFINITY)
		if (rlim.rlim_max == RLIM_INFINITY)
			retval = -1LL;
		else
			retval = rlim.rlim_max; 
	else
		retval = rlim.rlim_cur;

	elog(DEBUG5, "pg_setrlimit cur:%llu max:%llu return:%lld",
		(long long unsigned int) rlim.rlim_cur,
		(long long unsigned int) rlim.rlim_max, (long long int) retval);

	PG_RETURN_INT64(retval);
}

