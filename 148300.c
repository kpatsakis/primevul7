ZEND_API int ZEND_FASTCALL sub_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (sub_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	} else {
		return sub_function_slow(result, op1, op2);
	}
}