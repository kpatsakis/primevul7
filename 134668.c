static void end_write(TsHashTable *ht)
{
#ifdef ZTS
	tsrm_mutex_unlock(ht->mx_writer);
#endif
}