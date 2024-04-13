ZEND_API int zend_ts_hash_find(TsHashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_find(TS_HASH(ht), arKey, nKeyLength, pData);
	end_read(ht);

	return retval;
}