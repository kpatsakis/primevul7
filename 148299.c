static zend_never_inline zval* ZEND_FASTCALL _zendi_convert_scalar_to_number_noisy(zval *op, zval *holder) /* {{{ */
{
	return _zendi_convert_scalar_to_number_ex(op, holder, 0);
}