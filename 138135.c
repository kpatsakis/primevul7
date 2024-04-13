*/
PHP_FUNCTION(timezone_open)
{
	char *tz;
	int   tz_len;
	timelib_tzinfo *tzi = NULL;
	php_timezone_obj *tzobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (SUCCESS != timezone_initialize(&tzi, tz TSRMLS_CC)) {
		RETURN_FALSE;
	}
	tzobj = zend_object_store_get_object(php_date_instantiate(date_ce_timezone, return_value TSRMLS_CC) TSRMLS_CC);
	tzobj->type = TIMELIB_ZONETYPE_ID;
	tzobj->tzi.tz = tzi;
	tzobj->initialized = 1;