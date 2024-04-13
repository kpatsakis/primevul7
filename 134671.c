ZEND_API void zend_ts_hash_destroy(TsHashTable *ht)
{
	begin_write(ht);
	zend_hash_destroy(TS_HASH(ht));
	end_write(ht);

#ifdef ZTS
	tsrm_mutex_free(ht->mx_reader);
	tsrm_mutex_free(ht->mx_writer);
#endif
}