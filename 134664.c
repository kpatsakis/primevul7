ZEND_API int _zend_ts_hash_init_ex(TsHashTable *ht, uint nSize, dtor_func_t pDestructor, zend_bool persistent, zend_bool bApplyProtection ZEND_FILE_LINE_DC)
{
#ifdef ZTS
	ht->mx_reader = tsrm_mutex_alloc();
	ht->mx_writer = tsrm_mutex_alloc();
	ht->reader = 0;
#endif
	return _zend_hash_init_ex(TS_HASH(ht), nSize, pDestructor, persistent, bApplyProtection ZEND_FILE_LINE_RELAY_CC);
}