static zval *spl_array_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type TSRMLS_CC);
} /* }}} */