*/
PHP_METHOD(DateInterval, __construct)
{
	char *interval_string = NULL;
	int   interval_string_length;
	php_interval_obj *diobj;
	timelib_rel_time *reltime;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &interval_string, &interval_string_length) == SUCCESS) {
		if (date_interval_initialize(&reltime, interval_string, interval_string_length TSRMLS_CC) == SUCCESS) {
			diobj = zend_object_store_get_object(getThis() TSRMLS_CC);
			diobj->diff = reltime;
			diobj->initialized = 1;
		} else {
			ZVAL_NULL(getThis());
		}
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);