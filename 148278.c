ZEND_API int ZEND_FASTCALL mod_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, ZEND_MOD, mod_function);

	if (op2_lval == 0) {
		/* modulus by zero */
		if (EG(current_execute_data) && !CG(in_compilation)) {
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		} else {
			zend_error_noreturn(E_ERROR, "Modulo by zero");
		}
		if (op1 != result) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}

	if (op2_lval == -1) {
		/* Prevent overflow error/crash if op1==LONG_MIN */
		ZVAL_LONG(result, 0);
		return SUCCESS;
	}

	ZVAL_LONG(result, op1_lval % op2_lval);
	return SUCCESS;
}