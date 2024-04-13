ZEND_API ulong zend_ts_get_hash_value(TsHashTable *ht, char *arKey, uint nKeyLength)
{
	ulong retval;

	begin_read(ht);
	retval = zend_get_hash_value(arKey, nKeyLength);
	end_read(ht);

	return retval;
}