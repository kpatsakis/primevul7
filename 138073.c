/* {{{ date_period_write_property */
static void date_period_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Writing to DatePeriod properties is unsupported");