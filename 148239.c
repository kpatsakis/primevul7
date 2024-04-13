ZEND_API int ZEND_FASTCALL is_smaller_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (compare_function(result, op1, op2) == FAILURE) {
		return FAILURE;
	}
	ZVAL_BOOL(result, (Z_LVAL_P(result) < 0));
	return SUCCESS;
}