SPL_METHOD(SplDoublyLinkedList, bottom)
{
	zval *value;
	spl_dllist_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	value  = (zval *)spl_ptr_llist_first(intern->llist);

	if (value == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Can't peek at an empty datastructure", 0 TSRMLS_CC);
		return;
	}

	RETURN_ZVAL(value, 1, 0);
}