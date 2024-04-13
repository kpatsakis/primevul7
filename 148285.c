ZEND_API int ZEND_FASTCALL bitwise_and_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG) && EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		ZVAL_LONG(result, Z_LVAL_P(op1) & Z_LVAL_P(op2));
		return SUCCESS;
	}

	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		zend_string *str;
		size_t i;

		if (EXPECTED(Z_STRLEN_P(op1) >= Z_STRLEN_P(op2))) {
			if (EXPECTED(Z_STRLEN_P(op1) == Z_STRLEN_P(op2)) && Z_STRLEN_P(op1) == 1) {
				zend_uchar and = (zend_uchar) (*Z_STRVAL_P(op1) & *Z_STRVAL_P(op2));
				if (result==op1) {
					zval_ptr_dtor_str(result);
				}
				ZVAL_INTERNED_STR(result, ZSTR_CHAR(and));
				return SUCCESS;
			}
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		str = zend_string_alloc(Z_STRLEN_P(shorter), 0);
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			ZSTR_VAL(str)[i] = Z_STRVAL_P(shorter)[i] & Z_STRVAL_P(longer)[i];
		}
		ZSTR_VAL(str)[i] = 0;
		if (result==op1) {
			zval_ptr_dtor_str(result);
		}
		ZVAL_NEW_STR(result, str);
		return SUCCESS;
	}

	if (UNEXPECTED(Z_TYPE_P(op1) != IS_LONG)) {
		ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BW_AND, bitwise_and_function);
		op1_lval = _zval_get_long_func_noisy(op1);
		if (UNEXPECTED(EG(exception))) {
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op1_lval = Z_LVAL_P(op1);
	}
	if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {
		ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BW_AND);
		op2_lval = _zval_get_long_func_noisy(op2);
		if (UNEXPECTED(EG(exception))) {
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op2_lval = Z_LVAL_P(op2);
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}
	ZVAL_LONG(result, op1_lval & op2_lval);
	return SUCCESS;
}