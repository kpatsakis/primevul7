ZEND_API void zend_ts_hash_copy_to_hash(HashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size)
{
	begin_read(source);
	zend_hash_copy(target, TS_HASH(source), pCopyConstructor, tmp, size);
	end_read(source);
}