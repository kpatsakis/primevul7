static zend_long ZEND_FASTCALL _zval_get_long_func_noisy(zval *op) /* {{{ */
{
	return _zval_get_long_func_ex(op, 0);
}