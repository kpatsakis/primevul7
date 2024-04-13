static int spl_array_has_property(zval *object, zval *member, int has_set_exists, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2, key TSRMLS_CC)) {
		return spl_array_has_dimension(object, member, has_set_exists TSRMLS_CC);
	}
	return std_object_handlers.has_property(object, member, has_set_exists, key TSRMLS_CC);
} /* }}} */