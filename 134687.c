ZEND_API void zend_ts_hash_copy(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, void *tmp, uint size)
{
	begin_read(source);
	begin_write(target);
	zend_hash_copy(TS_HASH(target), TS_HASH(source), pCopyConstructor, tmp, size);
	end_write(target);
	end_read(source);
}