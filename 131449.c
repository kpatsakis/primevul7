static void spl_array_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_write_dimension_ex(1, object, offset, value TSRMLS_CC);
} /* }}} */