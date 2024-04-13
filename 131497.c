static zval *spl_array_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2, key TSRMLS_CC)) {
		return spl_array_read_dimension(object, member, type TSRMLS_CC);
	}
	return std_object_handlers.read_property(object, member, type, key TSRMLS_CC);
} /* }}} */