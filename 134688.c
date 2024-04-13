ZEND_API int _zend_ts_hash_quick_add_or_update(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, void *pData, uint nDataSize, void **pDest, int flag ZEND_FILE_LINE_DC)
{
	int retval;

	begin_write(ht);
	retval = _zend_hash_quick_add_or_update(TS_HASH(ht), arKey, nKeyLength, h, pData, nDataSize, pDest, flag ZEND_FILE_LINE_RELAY_CC);
	end_write(ht);

	return retval;
}