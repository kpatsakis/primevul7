ZEND_API int ZEND_FASTCALL boolean_xor_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	int op1_val, op2_val;

	do {
		if (Z_TYPE_P(op1) == IS_FALSE) {
			op1_val = 0;
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
			op1_val = 1;
		} else {
			if (Z_ISREF_P(op1)) {
				op1 = Z_REFVAL_P(op1);
				if (Z_TYPE_P(op1) == IS_FALSE) {
					op1_val = 0;
					break;
				} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
					op1_val = 1;
					break;
				}
			}
			ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BOOL_XOR, boolean_xor_function);
			op1_val = zval_is_true(op1);
		}
	} while (0);
	do {
		if (Z_TYPE_P(op2) == IS_FALSE) {
			op2_val = 0;
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_TRUE)) {
			op2_val = 1;
		} else {
			if (Z_ISREF_P(op2)) {
				op2 = Z_REFVAL_P(op2);
				if (Z_TYPE_P(op2) == IS_FALSE) {
					op2_val = 0;
					break;
				} else if (EXPECTED(Z_TYPE_P(op2) == IS_TRUE)) {
					op2_val = 1;
					break;
				}
			}
			ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BOOL_XOR);
			op2_val = zval_is_true(op2);
		}
	} while (0);

	ZVAL_BOOL(result, op1_val ^ op2_val);
	return SUCCESS;
}