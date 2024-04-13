static int hash_zval_identical_function(zval *z1, zval *z2) /* {{{ */
{
	/* is_identical_function() returns 1 in case of identity and 0 in case
	 * of a difference;
	 * whereas this comparison function is expected to return 0 on identity,
	 * and non zero otherwise.
	 */
	ZVAL_DEREF(z1);
	ZVAL_DEREF(z2);
	return fast_is_not_identical_function(z1, z2);
}