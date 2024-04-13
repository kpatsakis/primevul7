ZEND_API int ZEND_FASTCALL string_compare_function_ex(zval *op1, zval *op2, zend_bool case_insensitive) /* {{{ */
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
	int ret;

	if (case_insensitive) {
		ret = zend_binary_strcasecmp_l(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str1));
	} else {
		ret = zend_binary_strcmp(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));
	}

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return ret;
}