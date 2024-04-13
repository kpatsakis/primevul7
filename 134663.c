static void begin_write(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_lock(ht->mx_writer);
#endif
}