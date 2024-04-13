/* {{{ Helper functions */
static char* guess_timezone(const timelib_tzdb *tzdb TSRMLS_DC)
{
	/* Checking configure timezone */
	if (DATEG(timezone) && (strlen(DATEG(timezone))) > 0) {
		return DATEG(timezone);
	}
	/* Check config setting for default timezone */
	if (!DATEG(default_timezone)) {
		/* Special case: ext/date wasn't initialized yet */
		zval ztz;

		if (SUCCESS == zend_get_configuration_directive("date.timezone", sizeof("date.timezone"), &ztz)
			&& Z_TYPE(ztz) == IS_STRING && Z_STRLEN(ztz) > 0 && timelib_timezone_id_is_valid(Z_STRVAL(ztz), tzdb)) {
			return Z_STRVAL(ztz);
		}
	} else if (*DATEG(default_timezone)) {
		if (DATEG(timezone_valid) == 1) {
			return DATEG(default_timezone);
		}

		if (!timelib_timezone_id_is_valid(DATEG(default_timezone), tzdb)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid date.timezone value '%s', we selected the timezone 'UTC' for now.", DATEG(default_timezone));
			return "UTC";
		}

		DATEG(timezone_valid) = 1;
		return DATEG(default_timezone);
	}
	/* Fallback to UTC */
	php_error_docref(NULL TSRMLS_CC, E_WARNING, DATE_TZ_ERRMSG "We selected the timezone 'UTC' for now, but please set date.timezone to select your timezone.");
	return "UTC";