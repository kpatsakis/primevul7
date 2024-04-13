ZEND_API void ZEND_FASTCALL _convert_to_cstring(zval *op) /* {{{ */
{
	if (Z_TYPE_P(op) == IS_DOUBLE) {
		zend_string *str;
		double dval = Z_DVAL_P(op);

		str = zend_strpprintf_unchecked(0, "%.*H", (int) EG(precision), dval);
		ZVAL_NEW_STR(op, str);
	} else {
		_convert_to_string(op);
	}
}