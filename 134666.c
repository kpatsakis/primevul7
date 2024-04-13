ZEND_API int zend_ts_hash_del_key_or_index(TsHashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_del_key_or_index(TS_HASH(ht), arKey, nKeyLength, h, flag);
	end_write(ht);

	return retval;
}