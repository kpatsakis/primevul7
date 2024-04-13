SPL_METHOD(Array, setIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zend_class_entry * ce_get_iterator = spl_ce_Iterator;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "C", &ce_get_iterator) == FAILURE) {
		return;
	}

	intern->ce_get_iterator = ce_get_iterator;
}