ZEND_API void zend_ts_hash_clean(TsHashTable *ht)
{
	ht->reader = 0;
	begin_write(ht);
	zend_hash_clean(TS_HASH(ht));
	end_write(ht);
}