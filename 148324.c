static zend_never_inline void ZEND_FASTCALL add_function_array(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (result == op1 && Z_ARR_P(op1) == Z_ARR_P(op2)) {
		/* $a += $a */
		return;
	}
	if (result != op1) {
		ZVAL_ARR(result, zend_array_dup(Z_ARR_P(op1)));
	} else {
		SEPARATE_ARRAY(result);
	}
	zend_hash_merge(Z_ARRVAL_P(result), Z_ARRVAL_P(op2), zval_add_ref, 0);
}