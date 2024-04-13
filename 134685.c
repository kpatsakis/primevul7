ZEND_API int zend_ts_hash_minmax(TsHashTable *ht, compare_func_t compar, int flag, void **pData TSRMLS_DC)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_minmax(TS_HASH(ht), compar, flag, pData TSRMLS_CC);
	end_read(ht);

	return retval;
}