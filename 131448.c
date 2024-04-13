SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval **array;
	long ar_flags = 0;
	zend_class_entry *ce_get_iterator = spl_ce_Iterator;
	zend_error_handling error_handling;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

	intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lC", &array, &ar_flags, &ce_get_iterator) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	if (ZEND_NUM_ARGS() > 2) {
		intern->ce_get_iterator = ce_get_iterator;
	}

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1 TSRMLS_CC);

	zend_restore_error_handling(&error_handling TSRMLS_CC);

}