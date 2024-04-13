ZEND_API int zend_ts_hash_quick_find(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void **pData)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_quick_find(TS_HASH(ht), arKey, nKeyLength, h, pData);
	end_read(ht);

	return retval;
}