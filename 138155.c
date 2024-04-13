*/
PHP_METHOD(DateTime, __wakeup)
{
	zval             *object = getThis();
	php_date_obj     *dateobj;
	HashTable        *myht;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	myht = Z_OBJPROP_P(object);

	if (!php_date_initialize_from_hash(&dateobj, myht TSRMLS_CC)) {
		php_error(E_ERROR, "Invalid serialization data for DateTime object");
	}