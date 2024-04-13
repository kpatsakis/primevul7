ZEND_API int _zend_ts_hash_index_update_or_next_insert(TsHashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC)
{
	int retval;

	begin_write(ht);
	retval = _zend_hash_index_update_or_next_insert(TS_HASH(ht), h, pData, nDataSize, pDest, flag ZEND_FILE_LINE_RELAY_CC);
	end_write(ht);

	return retval;
}