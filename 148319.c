static zend_always_inline zval* _zendi_convert_scalar_to_number_ex(zval *op, zval *holder, zend_bool silent) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(holder, 0);
			return holder;
		case IS_TRUE:
			ZVAL_LONG(holder, 1);
			return holder;
		case IS_STRING:
			if ((Z_TYPE_INFO_P(holder) = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &Z_LVAL_P(holder), &Z_DVAL_P(holder), silent ? 1 : -1)) == 0) {
				ZVAL_LONG(holder, 0);
				if (!silent) {
					zend_error(E_WARNING, "A non-numeric value encountered");
				}
			}
			return holder;
		case IS_RESOURCE:
			ZVAL_LONG(holder, Z_RES_HANDLE_P(op));
			return holder;
		case IS_OBJECT:
			convert_object_to_type(op, holder, _IS_NUMBER, convert_scalar_to_number);
			if (UNEXPECTED(EG(exception)) ||
			    UNEXPECTED(Z_TYPE_P(holder) != IS_LONG && Z_TYPE_P(holder) != IS_DOUBLE)) {
				ZVAL_LONG(holder, 1);
			}
			return holder;
		case IS_LONG:
		case IS_DOUBLE:
		default:
			return op;
	}
}