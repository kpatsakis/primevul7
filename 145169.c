static PHP_INI_MH(OnSetPrecision)
{
	zend_long i;

	ZEND_ATOL(i, ZSTR_VAL(new_value));
	if (i >= 0) {
		EG(precision) = i;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}