ZEND_API void ZEND_FASTCALL convert_to_array(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			break;
/* OBJECTS_OPTIMIZE */
		case IS_OBJECT:
			if (Z_OBJCE_P(op) == zend_ce_closure) {
				convert_scalar_to_array(op);
			} else {
				HashTable *obj_ht = zend_get_properties_for(op, ZEND_PROP_PURPOSE_ARRAY_CAST);
				if (obj_ht) {
					HashTable *new_obj_ht = zend_proptable_to_symtable(obj_ht,
						(Z_OBJCE_P(op)->default_properties_count ||
						 Z_OBJ_P(op)->handlers != &std_object_handlers ||
						 GC_IS_RECURSIVE(obj_ht)));
					zval_ptr_dtor(op);
					ZVAL_ARR(op, new_obj_ht);
					zend_release_properties(obj_ht);
				} else {
					zval_ptr_dtor(op);
					/*ZVAL_EMPTY_ARRAY(op);*/
					array_init(op);
				}
			}
			break;
		case IS_NULL:
			/*ZVAL_EMPTY_ARRAY(op);*/
			array_init(op);
			break;
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		default:
			convert_scalar_to_array(op);
			break;
	}
}