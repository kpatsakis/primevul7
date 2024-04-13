static int spl_array_next_ex(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos_ex(intern, aht TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return FAILURE;
	}

	return spl_array_next_no_verify(intern, aht TSRMLS_CC);
} /* }}} */