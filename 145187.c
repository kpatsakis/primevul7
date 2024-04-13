static PHP_INI_MH(OnChangeMemoryLimit)
{
	if (new_value) {
		PG(memory_limit) = zend_atol(ZSTR_VAL(new_value), (int)ZSTR_LEN(new_value));
	} else {
		PG(memory_limit) = 1<<30;		/* effectively, no limit */
	}
	return zend_set_memory_limit(PG(memory_limit));
}