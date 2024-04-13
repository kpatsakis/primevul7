ZEND_API void ZEND_FASTCALL _convert_to_string(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE: {
			ZVAL_EMPTY_STRING(op);
			break;
		}
		case IS_TRUE:
			ZVAL_INTERNED_STR(op, ZSTR_CHAR('1'));
			break;
		case IS_STRING:
			break;
		case IS_RESOURCE: {
			zend_string *str = zend_strpprintf(0, "Resource id #" ZEND_LONG_FMT, (zend_long)Z_RES_HANDLE_P(op));
			zval_ptr_dtor(op);
			ZVAL_NEW_STR(op, str);
			break;
		}
		case IS_LONG: {
			ZVAL_STR(op, zend_long_to_str(Z_LVAL_P(op)));
			break;
		}
		case IS_DOUBLE: {
			zend_string *str;
			double dval = Z_DVAL_P(op);

			str = zend_strpprintf(0, "%.*G", (int) EG(precision), dval);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			ZVAL_NEW_STR(op, str);
			break;
		}
		case IS_ARRAY:
			zend_error(E_NOTICE, "Array to string conversion");
			zval_ptr_dtor(op);
			ZVAL_INTERNED_STR(op, ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED));
			break;
		case IS_OBJECT: {
			zval tmp;

			if (Z_OBJ_HT_P(op)->cast_object) {
				if (Z_OBJ_HT_P(op)->cast_object(op, &tmp, IS_STRING) == SUCCESS) {
					zval_ptr_dtor(op);
					ZVAL_COPY_VALUE(op, &tmp);
					return;
				}
			} else if (Z_OBJ_HT_P(op)->get) {
				zval *z = Z_OBJ_HT_P(op)->get(op, &tmp);
				if (Z_TYPE_P(z) != IS_OBJECT) {
					zend_string *str = zval_get_string(z);
					zval_ptr_dtor(z);
					zval_ptr_dtor(op);
					ZVAL_STR(op, str);
					return;
				}
				zval_ptr_dtor(z);
			}
			if (!EG(exception)) {
				zend_throw_error(NULL, "Object of class %s could not be converted to string", ZSTR_VAL(Z_OBJCE_P(op)->name));
			}
			zval_ptr_dtor(op);
			ZVAL_EMPTY_STRING(op);
			break;
		}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}