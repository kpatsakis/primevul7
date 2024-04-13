ZEND_API void ZEND_FASTCALL convert_scalar_to_number(zval *op) /* {{{ */
{
	_convert_scalar_to_number(op, 1, 0);
}