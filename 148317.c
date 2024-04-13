ZEND_API zend_long ZEND_FASTCALL zval_get_long_func(zval *op) /* {{{ */
{
	return _zval_get_long_func_ex(op, 1);
}