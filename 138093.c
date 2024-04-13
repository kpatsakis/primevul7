*/
PHP_FUNCTION(date_interval_format)
{
	zval             *object;
	php_interval_obj *diobj;
	char             *format;
	int               format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_interval, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	diobj = (php_interval_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(diobj->initialized, DateInterval);

	RETURN_STRING(date_interval_format(format, format_len, diobj->diff), 0);