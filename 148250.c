static zend_never_inline int ZEND_FASTCALL sub_function_slow(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy, op2_copy;
	int converted = 0;
	while (1) {
		if (Z_ISREF_P(op1)) {
			op1 = Z_REFVAL_P(op1);
		} else if (Z_ISREF_P(op2)) {
			op2 = Z_REFVAL_P(op2);
		} else if (!converted) {
			ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_SUB, sub_function);

			if (EXPECTED(op1 != op2)) {
				op1 = zendi_convert_scalar_to_number(op1, &op1_copy, result, 0);
				op2 = zendi_convert_scalar_to_number(op2, &op2_copy, result, 0);
			} else {
				op1 = zendi_convert_scalar_to_number(op1, &op1_copy, result, 0);
				op2 = op1;
			}
			if (EG(exception)) {
				if (result != op1) {
					ZVAL_UNDEF(result);
				}
				return FAILURE;
			}
			converted = 1;
		} else {
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			zend_throw_error(NULL, "Unsupported operand types");
			return FAILURE; /* unknown datatype */
		}
		if (sub_function_fast(result, op1, op2) == SUCCESS) {
			return SUCCESS;
		}
	}

}