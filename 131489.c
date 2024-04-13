static void spl_array_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2, key TSRMLS_CC)) {
		spl_array_write_dimension(object, member, value TSRMLS_CC);
		return;
	}
	std_object_handlers.write_property(object, member, value, key TSRMLS_CC);
} /* }}} */