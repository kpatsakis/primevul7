*/
PHP_FUNCTION(date_interval_create_from_date_string)
{
	char           *time_str = NULL;
	int             time_str_len = 0;
	timelib_time   *time;
	timelib_error_container *err = NULL;
	php_interval_obj *diobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &time_str, &time_str_len) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_instantiate(date_ce_interval, return_value TSRMLS_CC);

	time = timelib_strtotime(time_str, time_str_len, &err, DATE_TIMEZONEDB, php_date_parse_tzfile_wrapper);
	diobj = (php_interval_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	diobj->diff = timelib_rel_time_clone(&time->relative);
	diobj->initialized = 1;
	timelib_time_dtor(time);
	timelib_error_container_dtor(err);