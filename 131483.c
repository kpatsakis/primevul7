SPL_METHOD(Array, append)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}
	spl_array_iterator_append(getThis(), value TSRMLS_CC);
} /* }}} */