ZEND_API void zend_ts_hash_merge_ex(TsHashTable *target, TsHashTable *source, copy_ctor_func_t pCopyConstructor, uint size, merge_checker_func_t pMergeSource, void *pParam)
{
	begin_read(source);
	begin_write(target);
	zend_hash_merge_ex(TS_HASH(target), TS_HASH(source), pCopyConstructor, size, pMergeSource, pParam);
	end_write(target);
	end_read(source);
}