ZEND_API int zend_ts_hash_index_find(TsHashTable *ht, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_index_find(TS_HASH(ht), h, pData);
	end_read(ht);

	return retval;
}