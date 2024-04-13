ZEND_API int zend_ts_hash_add_empty_element(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_add_empty_element(TS_HASH(ht), arKey, nKeyLength);
	end_write(ht);

	return retval;
}