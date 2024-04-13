static void spl_array_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_array_unset_dimension_ex(1, object, offset TSRMLS_CC);
} /* }}} */