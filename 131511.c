static int spl_array_next(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	return spl_array_next_ex(intern, aht TSRMLS_CC);

} /* }}} */