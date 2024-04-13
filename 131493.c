static void spl_array_rewind(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable          *aht      = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::rewind(): Array was modified outside object and is no longer an array");
		return;
	}

	spl_array_rewind_ex(intern, aht TSRMLS_CC);
}