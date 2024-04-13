SPL_METHOD(SplDoublyLinkedList, offsetSet)
{
	zval                  *zindex, *value;
	spl_dllist_object     *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zindex, &value) == FAILURE) {
		return;
	}
	SEPARATE_ARG_IF_REF(value);

	intern = (spl_dllist_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (Z_TYPE_P(zindex) == IS_NULL) {
		/* $obj[] = ... */
		spl_ptr_llist_push(intern->llist, value TSRMLS_CC);
	} else {
		/* $obj[$foo] = ... */
		long                   index;
		spl_ptr_llist_element *element;

		index = spl_offset_convert_to_long(zindex TSRMLS_CC);

		if (index < 0 || index >= intern->llist->count) {
			zval_ptr_dtor(&value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid or out of range", 0 TSRMLS_CC);
			return;
		}

		element = spl_ptr_llist_offset(intern->llist, index, intern->flags & SPL_DLLIST_IT_LIFO);

		if (element != NULL) {
			/* call dtor on the old element as in spl_ptr_llist_pop */
			if (intern->llist->dtor) {
				intern->llist->dtor(element TSRMLS_CC);
			}

			/* the element is replaced, delref the old one as in
			 * SplDoublyLinkedList::pop() */
			zval_ptr_dtor((zval **)&element->data);
			element->data = value;

			/* new element, call ctor as in spl_ptr_llist_push */
			if (intern->llist->ctor) {
				intern->llist->ctor(element TSRMLS_CC);
			}
		} else {
			zval_ptr_dtor(&value);
			zend_throw_exception(spl_ce_OutOfRangeException, "Offset invalid", 0 TSRMLS_CC);
			return;
		}
	}
} /* }}} */