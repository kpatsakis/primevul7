ZEND_API int ZEND_FASTCALL pow_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

		if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
			if (Z_LVAL_P(op2) >= 0) {
				zend_long l1 = 1, l2 = Z_LVAL_P(op1), i = Z_LVAL_P(op2);

				if (i == 0) {
					ZVAL_LONG(result, 1L);
					return SUCCESS;
				} else if (l2 == 0) {
					ZVAL_LONG(result, 0);
					return SUCCESS;
				}

				while (i >= 1) {
					zend_long overflow;
					double dval = 0.0;

					if (i % 2) {
						--i;
						ZEND_SIGNED_MULTIPLY_LONG(l1, l2, l1, dval, overflow);
						if (overflow) {
							ZVAL_DOUBLE(result, dval * pow(l2, i));
							return SUCCESS;
						}
					} else {
						i /= 2;
						ZEND_SIGNED_MULTIPLY_LONG(l2, l2, l2, dval, overflow);
						if (overflow) {
							ZVAL_DOUBLE(result, (double)l1 * pow(dval, i));
							return SUCCESS;
						}
					}
				}
				/* i == 0 */
				ZVAL_LONG(result, l1);
			} else {
				ZVAL_DOUBLE(result, pow((double)Z_LVAL_P(op1), (double)Z_LVAL_P(op2)));
			}
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
			ZVAL_DOUBLE(result, pow(Z_DVAL_P(op1), Z_DVAL_P(op2)));
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
			ZVAL_DOUBLE(result, pow((double)Z_LVAL_P(op1), Z_DVAL_P(op2)));
			return SUCCESS;

		} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
			ZVAL_DOUBLE(result, pow(Z_DVAL_P(op1), (double)Z_LVAL_P(op2)));
			return SUCCESS;

		} else {
			if (Z_ISREF_P(op1)) {
				op1 = Z_REFVAL_P(op1);
			} else if (Z_ISREF_P(op2)) {
				op2 = Z_REFVAL_P(op2);
			} else if (!converted) {
				ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_POW, pow_function);

				if (EXPECTED(op1 != op2)) {
					if (Z_TYPE_P(op1) == IS_ARRAY) {
						if (op1 == result) {
							zval_ptr_dtor(result);
						}
						ZVAL_LONG(result, 0);
						return SUCCESS;
					} else {
						op1 = zendi_convert_scalar_to_number(op1, &op1_copy, result, 0);
					}
					if (Z_TYPE_P(op2) == IS_ARRAY) {
						if (op1 == result) {
							zval_ptr_dtor(result);
						}
						ZVAL_LONG(result, 1L);
						return SUCCESS;
					} else {
						op2 = zendi_convert_scalar_to_number(op2, &op2_copy, result, 0);
					}
				} else {
					if (Z_TYPE_P(op1) == IS_ARRAY) {
						if (op1 == result) {
							zval_ptr_dtor(result);
						}
						ZVAL_LONG(result, 0);
						return SUCCESS;
					} else {
						op1 = zendi_convert_scalar_to_number(op1, &op1_copy, result, 0);
					}
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
				return FAILURE;
			}
		}
	}
}