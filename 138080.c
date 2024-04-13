
static int date_period_initialize(timelib_time **st, timelib_time **et, timelib_rel_time **d, long *recurrences, /*const*/ char *format, int format_length TSRMLS_DC)
{
	timelib_time     *b = NULL, *e = NULL;
	timelib_rel_time *p = NULL;
	int               r = 0;
	int               retval = 0;
	struct timelib_error_container *errors;

	timelib_strtointerval(format, format_length, &b, &e, &p, &r, &errors);
	
	if (errors->error_count > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad format (%s)", format);
		retval = FAILURE;
	} else {
		*st = b;
		*et = e;
		*d  = p;
		*recurrences = r;
		retval = SUCCESS;
	}
	timelib_error_container_dtor(errors);
	return retval;