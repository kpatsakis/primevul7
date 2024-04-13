ZEND_API void zend_ts_hash_apply_with_arguments(TsHashTable *ht TSRMLS_DC, apply_func_args_t apply_func, int num_args, ...)
{
	va_list args;

	va_start(args, num_args);
	begin_write(ht);
	zend_hash_apply_with_arguments(TS_HASH(ht) TSRMLS_CC, apply_func, num_args, args);
	end_write(ht);
	va_end(args);
}