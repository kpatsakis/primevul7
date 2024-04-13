static inline HashTable *spl_array_get_hash_table(spl_array_object* intern, int check_std_props TSRMLS_DC) { /* {{{ */
	if ((intern->ar_flags & SPL_ARRAY_IS_SELF) != 0) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return intern->std.properties;
	} else if ((intern->ar_flags & SPL_ARRAY_USE_OTHER) && (check_std_props == 0 || (intern->ar_flags & SPL_ARRAY_STD_PROP_LIST) == 0) && Z_TYPE_P(intern->array) == IS_OBJECT) {
		spl_array_object *other  = (spl_array_object*)zend_object_store_get_object(intern->array TSRMLS_CC);
		return spl_array_get_hash_table(other, check_std_props TSRMLS_CC);
	} else if ((intern->ar_flags & ((check_std_props ? SPL_ARRAY_STD_PROP_LIST : 0) | SPL_ARRAY_IS_SELF)) != 0) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return intern->std.properties;
	} else {
		return HASH_OF(intern->array);
	}
} /* }}} */