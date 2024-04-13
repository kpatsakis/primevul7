ZEND_API int zend_ts_hash_rehash(TsHashTable *ht)
{
	int retval;

	begin_write(ht);
	retval = zend_hash_rehash(TS_HASH(ht));
	end_write(ht);

	return retval;
}