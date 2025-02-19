*/
PHP_FUNCTION(date_create)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL;
	int             time_str_len = 0;
	zval            datetime_object;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sO!", &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_instantiate(date_ce_date, &datetime_object TSRMLS_CC);
	if (!php_date_initialize(zend_object_store_get_object(&datetime_object TSRMLS_CC), time_str, time_str_len, NULL, timezone_object, 0 TSRMLS_CC)) {
		zval_dtor(&datetime_object);
		RETURN_FALSE;
	} else {
		zval *datetime_object_ptr = &datetime_object;
		RETVAL_ZVAL(datetime_object_ptr, 0, 0);
	}