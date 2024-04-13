ZEND_API void ZEND_FASTCALL convert_to_object(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			{
				HashTable *ht = zend_symtable_to_proptable(Z_ARR_P(op));
				zend_object *obj;

				if (GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) {
					/* TODO: try not to duplicate immutable arrays as well ??? */
					ht = zend_array_dup(ht);
				} else if (ht != Z_ARR_P(op)) {
					zval_ptr_dtor(op);
				} else {
					GC_DELREF(ht);
				}
				obj = zend_objects_new(zend_standard_class_def);
				obj->properties = ht;
				ZVAL_OBJ(op, obj);
				break;
			}
		case IS_OBJECT:
			break;
		case IS_NULL:
			object_init(op);
			break;
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		default: {
			zval tmp;
			ZVAL_COPY_VALUE(&tmp, op);
			object_init(op);
			zend_hash_add_new(Z_OBJPROP_P(op), ZSTR_KNOWN(ZEND_STR_SCALAR), &tmp);
			break;
		}
	}
}