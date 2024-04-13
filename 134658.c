void zend_ts_hash_display(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display(TS_HASH(ht));
	end_read(ht);
}