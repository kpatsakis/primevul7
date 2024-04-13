static HashTable* spl_array_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(obj TSRMLS_CC);
	zval *tmp, *storage;
	int name_len;
	char *zname;
	zend_class_entry *base;

	*is_temp = 0;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (HASH_OF(intern->array) == intern->std.properties) {
		return intern->std.properties;
	} else {
		if (intern->debug_info == NULL) {
			ALLOC_HASHTABLE(intern->debug_info);
			ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(intern->std.properties) + 1, 0);
		}

		if (intern->debug_info->nApplyCount == 0) {
			zend_hash_clean(intern->debug_info);
			zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

			storage = intern->array;
			zval_add_ref(&storage);

			base = (Z_OBJ_HT_P(obj) == &spl_handler_ArrayIterator) ? spl_ce_ArrayIterator : spl_ce_ArrayObject;
			zname = spl_gen_private_prop_name(base, "storage", sizeof("storage")-1, &name_len TSRMLS_CC);
			zend_symtable_update(intern->debug_info, zname, name_len+1, &storage, sizeof(zval *), NULL);
			efree(zname);
		}

		return intern->debug_info;
	}
}