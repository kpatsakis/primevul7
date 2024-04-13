static void ZEND_FASTCALL convert_compare_result_to_long(zval *result) /* {{{ */
{
	if (Z_TYPE_P(result) == IS_DOUBLE) {
		ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
	} else {
		convert_to_long(result);
	}
}