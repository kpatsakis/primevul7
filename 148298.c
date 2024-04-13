ZEND_API int ZEND_FASTCALL bitwise_not_function(zval *result, zval *op1) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			ZVAL_LONG(result, ~Z_LVAL_P(op1));
			return SUCCESS;
		case IS_DOUBLE:
			ZVAL_LONG(result, ~zend_dval_to_lval(Z_DVAL_P(op1)));
			return SUCCESS;
		case IS_STRING: {
			size_t i;

			if (Z_STRLEN_P(op1) == 1) {
				zend_uchar not = (zend_uchar) ~*Z_STRVAL_P(op1);
				ZVAL_INTERNED_STR(result, ZSTR_CHAR(not));
			} else {
				ZVAL_NEW_STR(result, zend_string_alloc(Z_STRLEN_P(op1), 0));
				for (i = 0; i < Z_STRLEN_P(op1); i++) {
					Z_STRVAL_P(result)[i] = ~Z_STRVAL_P(op1)[i];
				}
				Z_STRVAL_P(result)[i] = 0;
			}
			return SUCCESS;
		}
		case IS_REFERENCE:
			op1 = Z_REFVAL_P(op1);
			goto try_again;
		default:
			ZEND_TRY_UNARY_OBJECT_OPERATION(ZEND_BW_NOT);

			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			zend_throw_error(NULL, "Unsupported operand types");
			return FAILURE;
	}
}