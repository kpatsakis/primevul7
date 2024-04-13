static void ZEND_FASTCALL _convert_scalar_to_number(zval *op, zend_bool silent, zend_bool check) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		case IS_STRING:
			{
				zend_string *str;

				str = Z_STR_P(op);
				if ((Z_TYPE_INFO_P(op)=is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), &Z_LVAL_P(op), &Z_DVAL_P(op), silent ? 1 : -1)) == 0) {
					ZVAL_LONG(op, 0);
					if (!silent) {
						zend_error(E_WARNING, "A non-numeric value encountered");
					}
				}
				zend_string_release_ex(str, 0);
				break;
			}
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(op, 0);
			break;
		case IS_TRUE:
			ZVAL_LONG(op, 1);
			break;
		case IS_RESOURCE:
			{
				zend_long l = Z_RES_HANDLE_P(op);
				zval_ptr_dtor(op);
				ZVAL_LONG(op, l);
			}
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, _IS_NUMBER, convert_scalar_to_number);
				if (check && UNEXPECTED(EG(exception))) {
					return;
				}
				zval_ptr_dtor(op);

				if (Z_TYPE(dst) == IS_LONG || Z_TYPE(dst) == IS_DOUBLE) {
					ZVAL_COPY_VALUE(op, &dst);
				} else {
					ZVAL_LONG(op, 1);
				}
			}
			break;
	}
}