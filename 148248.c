ZEND_API int ZEND_FASTCALL add_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (add_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	} else {
		return add_function_slow(result, op1, op2);
	}
}