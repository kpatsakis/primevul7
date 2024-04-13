ZEND_API int ZEND_FASTCALL increment_function(zval *op1) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			fast_long_increment_function(op1);
			break;
		case IS_DOUBLE:
			Z_DVAL_P(op1) = Z_DVAL_P(op1) + 1;
			break;
		case IS_NULL:
			ZVAL_LONG(op1, 1);
			break;
		case IS_STRING: {
				zend_long lval;
				double dval;

				switch (is_numeric_string(Z_STRVAL_P(op1), Z_STRLEN_P(op1), &lval, &dval, 0)) {
					case IS_LONG:
						zval_ptr_dtor_str(op1);
						if (lval == ZEND_LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
							ZVAL_LONG(op1, lval+1);
						}
						break;
					case IS_DOUBLE:
						zval_ptr_dtor_str(op1);
						ZVAL_DOUBLE(op1, dval+1);
						break;
					default:
						/* Perl style string increment */
						increment_string(op1);
						break;
				}
			}
			break;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(op1, get)
			   && Z_OBJ_HANDLER_P(op1, set)) {
				/* proxy object */
				zval rv;
				zval *val;

				val = Z_OBJ_HANDLER_P(op1, get)(op1, &rv);
				Z_TRY_ADDREF_P(val);
				increment_function(val);
				Z_OBJ_HANDLER_P(op1, set)(op1, val);
				zval_ptr_dtor(val);
			} else if (Z_OBJ_HANDLER_P(op1, do_operation)) {
				zval op2;
				int res;

				ZVAL_LONG(&op2, 1);
				res = Z_OBJ_HANDLER_P(op1, do_operation)(ZEND_ADD, op1, op1, &op2);

				return res;
			}
			return FAILURE;
		case IS_REFERENCE:
			op1 = Z_REFVAL_P(op1);
			goto try_again;
		default:
			return FAILURE;
	}
	return SUCCESS;
}