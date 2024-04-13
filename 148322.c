ZEND_API zend_string* ZEND_FASTCALL zval_try_get_string_func(zval *op) /* {{{ */
{
	return __zval_get_string_func(op, 1);
}