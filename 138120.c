   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	char *times, *initial_ts;
	int   time_len, error1, error2;
	struct timelib_error_container *error;
	long  preset_ts = 0, ts;

	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	tzi = get_timezone_info(TSRMLS_C);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sl", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have an initial timestamp */
		now = timelib_time_ctor();

		initial_ts = emalloc(25);
		snprintf(initial_ts, 24, "@%ld UTC", preset_ts);
		t = timelib_strtotime(initial_ts, strlen(initial_ts), NULL, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper); /* we ignore the error here, as this should never fail */
		timelib_update_ts(t, tzi);
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, t->sse);
		timelib_time_dtor(t);
		efree(initial_ts);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have no initial timestamp */
		now = timelib_time_ctor();
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) time(NULL));
	} else {
		RETURN_FALSE;
	}

	if (!time_len) {
		timelib_time_dtor(now);	
		RETURN_FALSE;
	}

	t = timelib_strtotime(times, time_len, &error, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	error1 = error->error_count;
	timelib_error_container_dtor(error);
	timelib_fill_holes(t, now, TIMELIB_NO_CLONE);
	timelib_update_ts(t, tzi);
	ts = timelib_date_to_int(t, &error2);

	timelib_time_dtor(now);
	timelib_time_dtor(t);

	if (error1 || error2) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}