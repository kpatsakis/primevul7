SPL_METHOD(Array, offsetGet)
{
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	value = spl_array_read_dimension_ex(0, getThis(), index, BP_VAR_R TSRMLS_CC);
	RETURN_ZVAL(value, 1, 0);
} /* }}} */