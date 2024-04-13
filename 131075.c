SPL_METHOD(SplDoublyLinkedList, count)
{
	long count;
	spl_dllist_object *intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	count = spl_ptr_llist_count(intern->llist);
	RETURN_LONG(count);
}