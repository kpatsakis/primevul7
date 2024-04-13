*/
PHP_METHOD(DatePeriod, __wakeup)
{
	zval             *object = getThis();
	php_period_obj   *period_obj;
	HashTable        *myht;

	period_obj = zend_object_store_get_object(object TSRMLS_CC);

	myht = Z_OBJPROP_P(object);

	if (!php_date_period_initialize_from_hash(period_obj, myht TSRMLS_CC)) {
		php_error(E_ERROR, "Invalid serialization data for DatePeriod object");
	}