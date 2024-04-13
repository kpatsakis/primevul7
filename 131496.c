static HashTable *spl_array_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *result;

	if (intern->nApplyCount > 1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Nesting level too deep - recursive dependency?");
	}

	intern->nApplyCount++;
	result = spl_array_get_hash_table(intern, 1 TSRMLS_CC);
	intern->nApplyCount--;
	return result;
} /* }}} */