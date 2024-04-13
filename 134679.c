ZEND_API int zend_ts_hash_num_elements(TsHashTable *ht)
{
	int retval;

	begin_read(ht);
	retval = zend_hash_num_elements(TS_HASH(ht));
	end_read(ht);

	return retval;
}