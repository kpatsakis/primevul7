ZEND_API int ZEND_FASTCALL mul_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

		if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
			zend_long overflow;

			ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1),Z_LVAL_P(op2), Z_LVAL_P(result),Z_DVAL_P(result),overflow);
			Z_TYPE_INFO_P(result) = overflow ? IS_DOUBLE : IS_LONG;
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
			ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) * Z_DVAL_P(op2));
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
			ZVAL_DOUBLE(result, Z_DVAL_P(op1) * ((double)Z_LVAL_P(op2)));
			return SUCCESS;

		} else {
			if (Z_ISREF_P(op1)) {
				op1 = Z_REFVAL_P(op1);
			} else if (Z_ISREF_P(op2)) {
				op2 = Z_REFVAL_P(op2);
			} else if (!converted) {
				ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_MUL, mul_function);

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
		}
	}
}