static inline int spl_array_object_verify_pos(spl_array_object *object, HashTable *ht TSRMLS_DC)
{
	return spl_array_object_verify_pos_ex(object, ht, "" TSRMLS_CC);
} /* }}} */