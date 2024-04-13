static zend_object_value spl_array_object_new_ex(zend_class_entry *class_type, spl_array_object **obj, zval *orig, int clone_orig TSRMLS_DC)
{
	zend_object_value retval = {0};
	spl_array_object *intern;
	zval *tmp;
	zend_class_entry * parent = class_type;
	int inherited = 0;

	intern = emalloc(sizeof(spl_array_object));
	memset(intern, 0, sizeof(spl_array_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->ar_flags = 0;
	intern->debug_info       = NULL;
	intern->ce_get_iterator = spl_ce_ArrayIterator;
	if (orig) {
		spl_array_object *other = (spl_array_object*)zend_object_store_get_object(orig TSRMLS_CC);

		intern->ar_flags &= ~ SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= (other->ar_flags & SPL_ARRAY_CLONE_MASK);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (clone_orig) {
			intern->array = other->array;
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayObject) {
				MAKE_STD_ZVAL(intern->array);
				array_init(intern->array);
				zend_hash_copy(HASH_OF(intern->array), HASH_OF(other->array), (copy_ctor_func_t) zval_add_ref, &tmp, sizeof(zval*));
			}
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayIterator) {
				Z_ADDREF_P(other->array);
			}
		} else {
			intern->array = orig;
			Z_ADDREF_P(intern->array);
			intern->ar_flags |= SPL_ARRAY_IS_REF | SPL_ARRAY_USE_OTHER;
		}
	} else {
		MAKE_STD_ZVAL(intern->array);
		array_init(intern->array);
		intern->ar_flags &= ~SPL_ARRAY_IS_REF;
	}

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_array_object_free_storage, NULL TSRMLS_CC);
	while (parent) {
		if (parent == spl_ce_ArrayIterator || parent == spl_ce_RecursiveArrayIterator) {
			retval.handlers = &spl_handler_ArrayIterator;
			class_type->get_iterator = spl_array_get_iterator;
			break;
		} else if (parent == spl_ce_ArrayObject) {
			retval.handlers = &spl_handler_ArrayObject;
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}
	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayObject or ArrayIterator");
	}
	if (inherited) {
		zend_hash_find(&class_type->function_table, "offsetget",    sizeof("offsetget"),    (void **) &intern->fptr_offset_get);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetset",    sizeof("offsetset"),    (void **) &intern->fptr_offset_set);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetexists", sizeof("offsetexists"), (void **) &intern->fptr_offset_has);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetunset",  sizeof("offsetunset"),  (void **) &intern->fptr_offset_del);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		zend_hash_find(&class_type->function_table, "count",        sizeof("count"),        (void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}
	/* Cache iterator functions if ArrayIterator or derived. Check current's */
	/* cache since only current is always required */
	if (retval.handlers == &spl_handler_ArrayIterator) {
		if (!class_type->iterator_funcs.zf_current) {
			zend_hash_find(&class_type->function_table, "rewind",  sizeof("rewind"),  (void **) &class_type->iterator_funcs.zf_rewind);
			zend_hash_find(&class_type->function_table, "valid",   sizeof("valid"),   (void **) &class_type->iterator_funcs.zf_valid);
			zend_hash_find(&class_type->function_table, "key",     sizeof("key"),     (void **) &class_type->iterator_funcs.zf_key);
			zend_hash_find(&class_type->function_table, "current", sizeof("current"), (void **) &class_type->iterator_funcs.zf_current);
			zend_hash_find(&class_type->function_table, "next",    sizeof("next"),    (void **) &class_type->iterator_funcs.zf_next);
		}
		if (inherited) {
			if (class_type->iterator_funcs.zf_rewind->common.scope  != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_REWIND;
			if (class_type->iterator_funcs.zf_valid->common.scope   != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_VALID;
			if (class_type->iterator_funcs.zf_key->common.scope     != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_KEY;
			if (class_type->iterator_funcs.zf_current->common.scope != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_CURRENT;
			if (class_type->iterator_funcs.zf_next->common.scope    != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_NEXT;
		}
	}

	spl_array_rewind(intern TSRMLS_CC);
	return retval;
}