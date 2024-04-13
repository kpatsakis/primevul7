ZEND_API int ZEND_FASTCALL shift_left_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, ZEND_SL, shift_left_function);

	/* prevent wrapping quirkiness on some processors where << 64 + x == << x */
	if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
		if (EXPECTED(op2_lval > 0)) {
			if (op1 == result) {
				zval_ptr_dtor(result);
			}
			ZVAL_LONG(result, 0);
			return SUCCESS;
		} else {
			if (EG(current_execute_data) && !CG(in_compilation)) {
				zend_throw_exception_ex(zend_ce_arithmetic_error, 0, "Bit shift by negative number");
			} else {
				zend_error_noreturn(E_ERROR, "Bit shift by negative number");
			}
			if (op1 != result) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}

	/* Perform shift on unsigned numbers to get well-defined wrap behavior. */
	ZVAL_LONG(result, (zend_long) ((zend_ulong) op1_lval << op2_lval));
	return SUCCESS;
}