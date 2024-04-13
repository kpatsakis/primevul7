*/
PHP_METHOD(DatePeriod, __set_state)
{
	php_period_obj   *period_obj;
	zval             *array;
	HashTable        *myht;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	myht = Z_ARRVAL_P(array);
	
	object_init_ex(return_value, date_ce_period);
	period_obj = zend_object_store_get_object(return_value TSRMLS_CC);
	if (!php_date_period_initialize_from_hash(period_obj, myht TSRMLS_CC)) {
		php_error(E_ERROR, "Invalid serialization data for DatePeriod object");
	}