*/
PHP_FUNCTION(date_parse_from_format)
{
	char                           *date, *format;
	int                             date_len, format_len;
	struct timelib_error_container *error;
	timelib_time                   *parsed_time;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &format, &format_len, &date, &date_len) == FAILURE) {
		RETURN_FALSE;
	}

	parsed_time = timelib_parse_from_format(format, date, date_len, &error, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	php_date_do_return_parsed_time(INTERNAL_FUNCTION_PARAM_PASSTHRU, parsed_time, error);