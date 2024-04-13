SPL_METHOD(SplDoublyLinkedList, offsetExists)
{
	zval              *zindex;
	spl_dllist_object *intern;
	long               index;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	index  = spl_offset_convert_to_long(zindex TSRMLS_CC);

	RETURN_BOOL(index >= 0 && index < intern->llist->count);
} /* }}} */