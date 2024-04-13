static void convert_scalar_to_array(zval *op) /* {{{ */
{
	HashTable *ht = zend_new_array(1);
	zend_hash_index_add_new(ht, 0, op);
	ZVAL_ARR(op, ht);
}