static int spl_array_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	HashTable			*ht1,
						*ht2;
	spl_array_object	*intern1,
						*intern2;
	int					result	= 0;
	zval				temp_zv;

	intern1	= (spl_array_object*)zend_object_store_get_object(o1 TSRMLS_CC);
	intern2	= (spl_array_object*)zend_object_store_get_object(o2 TSRMLS_CC);
	ht1		= spl_array_get_hash_table(intern1, 0 TSRMLS_CC);
	ht2		= spl_array_get_hash_table(intern2, 0 TSRMLS_CC);

	zend_compare_symbol_tables(&temp_zv, ht1, ht2 TSRMLS_CC);
	result = (int)Z_LVAL(temp_zv);
	/* if we just compared std.properties, don't do it again */
	if (result == 0 &&
			!(ht1 == intern1->std.properties && ht2 == intern2->std.properties)) {
		result = std_object_handlers.compare_objects(o1, o2 TSRMLS_CC);
	}
	return result;
} /* }}} */