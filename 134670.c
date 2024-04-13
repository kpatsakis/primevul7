ZEND_API int zend_ts_hash_sort(TsHashTable *ht, sort_func_t sort_func, compare_func_t compare_func, int renumber TSRMLS_DC)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_sort(TS_HASH(ht), sort_func, compare_func, renumber TSRMLS_CC);
	end_write(ht);

	return retval;
}