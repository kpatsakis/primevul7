static zend_always_inline zend_long ZEND_FASTCALL _zval_get_long_func_ex(zval *op, zend_bool silent) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_RESOURCE:
			return Z_RES_HANDLE_P(op);
		case IS_LONG:
			return Z_LVAL_P(op);
		case IS_DOUBLE:
			return zend_dval_to_lval(Z_DVAL_P(op));
		case IS_STRING:
			{
				zend_uchar type;
				zend_long lval;
				double dval;
				if (0 == (type = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &lval, &dval, silent ? 1 : -1))) {
					if (!silent) {
						zend_error(E_WARNING, "A non-numeric value encountered");
					}
					return 0;
				} else if (EXPECTED(type == IS_LONG)) {
					return lval;
				} else {
					/* Previously we used strtol here, not is_numeric_string,
					 * and strtol gives you LONG_MAX/_MIN on overflow.
					 * We use use saturating conversion to emulate strtol()'s
					 * behaviour.
					 */
					 return zend_dval_to_lval_cap(dval);
				}
			}
		case IS_ARRAY:
			return zend_hash_num_elements(Z_ARRVAL_P(op)) ? 1 : 0;
		case IS_OBJECT:
			{
				zval dst;
				convert_object_to_type(op, &dst, IS_LONG, convert_to_long);
				if (Z_TYPE(dst) == IS_LONG) {
					return Z_LVAL(dst);
				} else {
					return 1;
				}
			}
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 0;
}