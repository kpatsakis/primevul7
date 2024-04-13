*/
PHP_METHOD(DateTimeZone, __construct)
{
	char *tz;
	int tz_len;
	timelib_tzinfo *tzi = NULL;
	php_timezone_obj *tzobj;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len)) {
		if (SUCCESS == timezone_initialize(&tzi, tz TSRMLS_CC)) {
			tzobj = zend_object_store_get_object(getThis() TSRMLS_CC);
			tzobj->type = TIMELIB_ZONETYPE_ID;
			tzobj->tzi.tz = tzi;
			tzobj->initialized = 1;
		} else {
			ZVAL_NULL(getThis());
		}
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);