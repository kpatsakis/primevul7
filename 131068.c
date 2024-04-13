SPL_METHOD(SplDoublyLinkedList, isEmpty)
{
	long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	spl_dllist_object_count_elements(getThis(), &count TSRMLS_CC);
	RETURN_BOOL(count==0);
}