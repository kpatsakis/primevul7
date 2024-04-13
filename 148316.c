ZEND_API void ZEND_FASTCALL convert_to_null(zval *op) /* {{{ */
{
	zval_ptr_dtor(op);
	ZVAL_NULL(op);
}