ZEND_API int ZEND_FASTCALL is_identical_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, zend_is_identical(op1, op2));
	return SUCCESS;
}