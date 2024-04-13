static int spl_array_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	return spl_array_has_dimension_ex(1, object, offset, check_empty TSRMLS_CC);
} /* }}} */