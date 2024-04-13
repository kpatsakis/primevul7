ZEND_API int ZEND_FASTCALL boolean_not_function(zval *result, zval *op1) /* {{{ */
{
	if (Z_TYPE_P(op1) < IS_TRUE) {
		ZVAL_TRUE(result);
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
		ZVAL_FALSE(result);
	} else {
		if (Z_ISREF_P(op1)) {
			op1 = Z_REFVAL_P(op1);
			if (Z_TYPE_P(op1) < IS_TRUE) {
				ZVAL_TRUE(result);
				return SUCCESS;
			} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
				ZVAL_FALSE(result);
				return SUCCESS;
			}
		}
		ZEND_TRY_UNARY_OBJECT_OPERATION(ZEND_BOOL_NOT);

		ZVAL_BOOL(result, !zval_is_true(op1));
	}
	return SUCCESS;
}