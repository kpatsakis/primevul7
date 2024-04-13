*/
PHP_METHOD(DateInterval, __set_state)
{
	php_interval_obj *intobj;
	zval             *array;
	HashTable        *myht;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	myht = HASH_OF(array);

	php_date_instantiate(date_ce_interval, return_value TSRMLS_CC);
	intobj = (php_interval_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	php_date_interval_initialize_from_hash(&return_value, &intobj, myht TSRMLS_CC);