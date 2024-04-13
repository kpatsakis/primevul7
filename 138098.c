*/
PHP_METHOD(DateInterval, __wakeup)
{
	zval             *object = getThis();
	php_interval_obj *intobj;
	HashTable        *myht;

	intobj = (php_interval_obj *) zend_object_store_get_object(object TSRMLS_CC);

	myht = Z_OBJPROP_P(object);

	php_date_interval_initialize_from_hash(&return_value, &intobj, myht TSRMLS_CC);