static void spl_array_rewind_ex(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{

	zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
	spl_array_update_pos(intern);
	spl_array_skip_protected(intern, aht TSRMLS_CC);

} /* }}} */