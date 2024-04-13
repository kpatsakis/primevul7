static zend_always_inline int sub_function_fast(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		fast_long_sub_function(result, op1, op2);
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) - Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) - ((double)Z_LVAL_P(op2)));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}