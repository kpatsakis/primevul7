static void spl_array_unset_property(zval *object, zval *member, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2, key TSRMLS_CC)) {
		spl_array_unset_dimension(object, member TSRMLS_CC);
		spl_array_rewind(intern TSRMLS_CC); /* because deletion might invalidate position */
		return;
	}
	std_object_handlers.unset_property(object, member, key TSRMLS_CC);
} /* }}} */