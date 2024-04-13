ZEND_API int zend_ts_hash_exists(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_exists(TS_HASH(ht), arKey, nKeyLength);
	end_read(ht);

	return retval;
}