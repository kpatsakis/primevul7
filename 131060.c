SPL_METHOD(SplDoublyLinkedList, offsetGet)
{
	zval                  *zindex, *value;
	long                   index;
	spl_dllist_object     *intern;
	spl_ptr_llist_element *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	index  = spl_offset_convert_to_long(zindex TSRMLS_CC);

    if (index < 0 || index >= intern->llist->count) {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0 TSRMLS_CC);
		return;
	}

	element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

	if (element != NULL) {
		value = (zval *)element->data;
		RETURN_ZVAL(value, 1, 0);
	} else {
		zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
		return;
	}
} /* }}} */