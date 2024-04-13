static HashTable* spl_dllist_object_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{{ */
{
	spl_dllist_object     *intern  = (spl_dllist_object*)zend_object_store_get_object(obj TSRMLS_CC);
	spl_ptr_llist_element *current = intern->llist->head, *next;
	zval *tmp, zrv, *dllist_array;
	char *pnstr;
	int  pnlen;
	int  i = 0;

	*is_temp = 0;

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		zend_hash_init(intern->debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);
	}

	if (intern->debug_info->nApplyCount == 0) {
		INIT_PZVAL(&zrv);
		Z_ARRVAL(zrv) = intern->debug_info;

		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "flags", sizeof("flags")-1, &pnlen TSRMLS_CC);
		add_assoc_long_ex(&zrv, pnstr, pnlen+1, intern->flags);
		efree(pnstr);

		ALLOC_INIT_ZVAL(dllist_array);
		array_init(dllist_array);

		while (current) {
			next = current->next;

			add_index_zval(dllist_array, i, (zval *)current->data);
			Z_ADDREF_P(current->data);
			i++;

			current = next;
		}

		pnstr = spl_gen_private_prop_name(spl_ce_SplDoublyLinkedList, "dllist", sizeof("dllist")-1, &pnlen TSRMLS_CC);
		add_assoc_zval_ex(&zrv, pnstr, pnlen+1, dllist_array);
		efree(pnstr);
	}

	return intern->debug_info;
}