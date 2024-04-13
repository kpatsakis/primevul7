ZEND_API int ZEND_FASTCALL string_compare_function(zval *op1, zval *op2) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(op1) == IS_STRING) &&
	    EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		if (Z_STR_P(op1) == Z_STR_P(op2)) {
			return 0;
		} else {
			return zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		}
	} else {
		zend_string *tmp_str1, *tmp_str2;
		zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
		zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
		int ret = zend_binary_strcmp(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));

		zend_tmp_string_release(tmp_str1);
		zend_tmp_string_release(tmp_str2);
		return ret;
	}
}