
static int date_interval_initialize(timelib_rel_time **rt, /*const*/ char *format, int format_length TSRMLS_DC)
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
		if(p) {
			*rt = p;
			retval = SUCCESS;
		} else {
			if(b && e) {
				timelib_update_ts(b, NULL);
				timelib_update_ts(e, NULL);
				*rt = timelib_diff(b, e);
				retval = SUCCESS;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to parse interval (%s)", format);
				retval = FAILURE;
			}
		}
	}
	timelib_error_container_dtor(errors);
	return retval;