void zend_ts_hash_display_pListTail(TsHashTable *ht)
{
	begin_read(ht);
	zend_hash_display_pListTail(TS_HASH(ht));
	end_read(ht);
}