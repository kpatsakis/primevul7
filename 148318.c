ZEND_API zend_bool ZEND_FASTCALL _try_convert_to_string(zval *op)
{
	zend_string *str;

	ZEND_ASSERT(Z_TYPE_P(op) != IS_STRING);
	str = zval_try_get_string_func(op);
	if (UNEXPECTED(!str)) {
		return 0;
	}
	zval_ptr_dtor(op);
	ZVAL_STR(op, str);
	return 1;
}