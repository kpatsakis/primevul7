*/
PHP_FUNCTION(date_add)
{
	zval         *object, *interval;
	php_date_obj *dateobj;
	php_interval_obj *intobj;
	timelib_time     *new_time;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	new_time = timelib_add(dateobj->time, intobj->diff);
	timelib_time_dtor(dateobj->time);
	dateobj->time = new_time;

	RETURN_ZVAL(object, 1, 0);