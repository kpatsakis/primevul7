static zend_class_entry * spl_find_ce_by_name(char *name, int len, zend_bool autoload TSRMLS_DC)
{
	zend_class_entry **ce;
	int found;

	if (!autoload) {
		char *lc_name;
		ALLOCA_FLAG(use_heap)

		lc_name = do_alloca(len + 1, use_heap);
		zend_str_tolower_copy(lc_name, name, len);

		found = zend_hash_find(EG(class_table), lc_name, len +1, (void **) &ce);
		free_alloca(lc_name, use_heap);
	} else {
 		found = zend_lookup_class(name, len, &ce TSRMLS_CC);
 	}
 	if (found != SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Class %s does not exist%s", name, autoload ? " and could not be loaded" : "");
		return NULL;
	}
	
	return *ce;
}