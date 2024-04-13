static int spl_array_next_no_verify(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	zend_hash_move_forward_ex(aht, &intern->pos);
	spl_array_update_pos(intern);
	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		return spl_array_skip_protected(intern, aht TSRMLS_CC);
	} else {
		return zend_hash_has_more_elements_ex(aht, &intern->pos);
	}
} /* }}} */