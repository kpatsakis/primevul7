static inline int spl_array_object_verify_pos_ex(spl_array_object *object, HashTable *ht, const char *msg_prefix TSRMLS_DC)
{
	if (!ht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%sArray was modified outside object and is no longer an array", msg_prefix);
		return FAILURE;
	}

	if (object->pos && (object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos_ex(object, ht TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%sArray was modified outside object and internal position is no longer valid", msg_prefix);
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */