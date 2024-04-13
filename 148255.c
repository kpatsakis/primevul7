ZEND_API int ZEND_FASTCALL string_locale_compare_function(zval *op1, zval *op2) /* {{{ */
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
	int ret = strcoll(ZSTR_VAL(str1), ZSTR_VAL(str2));

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return ret;
}