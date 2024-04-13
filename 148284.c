ZEND_API int ZEND_FASTCALL numeric_compare_function(zval *op1, zval *op2) /* {{{ */
{
	double d1, d2;

	d1 = zval_get_double(op1);
	d2 = zval_get_double(op2);

	return ZEND_NORMALIZE_BOOL(d1 - d2);
}