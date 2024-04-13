ZEND_API void ZEND_FASTCALL convert_to_boolean(zval *op) /* {{{ */
{
	int tmp;

try_again:
	switch (Z_TYPE_P(op)) {
		case IS_FALSE:
		case IS_TRUE:
			break;
		case IS_NULL:
			ZVAL_FALSE(op);
			break;
		case IS_RESOURCE: {
				zend_long l = (Z_RES_HANDLE_P(op) ? 1 : 0);

				zval_ptr_dtor(op);
				ZVAL_BOOL(op, l);
			}
			break;
		case IS_LONG:
			ZVAL_BOOL(op, Z_LVAL_P(op) ? 1 : 0);
			break;
		case IS_DOUBLE:
			ZVAL_BOOL(op, Z_DVAL_P(op) ? 1 : 0);
			break;
		case IS_STRING:
			{
				zend_string *str = Z_STR_P(op);

				if (ZSTR_LEN(str) == 0
					|| (ZSTR_LEN(str) == 1 && ZSTR_VAL(str)[0] == '0')) {
					ZVAL_FALSE(op);
				} else {
					ZVAL_TRUE(op);
				}
				zend_string_release_ex(str, 0);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_ptr_dtor(op);
			ZVAL_BOOL(op, tmp);
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, _IS_BOOL, convert_to_boolean);
				zval_ptr_dtor(op);

				if (Z_TYPE_INFO(dst) == IS_FALSE || Z_TYPE_INFO(dst) == IS_TRUE) {
					Z_TYPE_INFO_P(op) = Z_TYPE_INFO(dst);
				} else {
					ZVAL_TRUE(op);
				}
				break;
			}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}