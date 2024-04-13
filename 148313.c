ZEND_API int ZEND_FASTCALL zend_object_is_true(zval *op) /* {{{ */
{
	if (Z_OBJ_HT_P(op)->cast_object) {
		zval tmp;
		if (Z_OBJ_HT_P(op)->cast_object(op, &tmp, _IS_BOOL) == SUCCESS) {
			return Z_TYPE(tmp) == IS_TRUE;
		}
		zend_error(E_RECOVERABLE_ERROR, "Object of class %s could not be converted to bool", ZSTR_VAL(Z_OBJ_P(op)->ce->name));
	} else if (Z_OBJ_HT_P(op)->get) {
		int result;
		zval rv;
		zval *tmp = Z_OBJ_HT_P(op)->get(op, &rv);

		if (Z_TYPE_P(tmp) != IS_OBJECT) {
			/* for safety - avoid loop */
			result = i_zend_is_true(tmp);
			zval_ptr_dtor(tmp);
			return result;
		}
	}
	return 1;
}