static int spl_array_skip_protected(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	char *string_key;
	uint string_length;
	ulong num_key;

	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		do {
			if (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 0, &intern->pos) == HASH_KEY_IS_STRING) {
				/* zend_hash_get_current_key_ex() should never set
				 * string_length to 0 when returning HASH_KEY_IS_STRING, but we
				 * may as well be defensive and consider that successful.
				 * Beyond that, we're looking for protected keys (which will
				 * have a null byte at string_key[0]), but want to avoid
				 * skipping completely empty keys (which will also have the
				 * null byte, but a string_length of 1). */
				if (!string_length || string_key[0] || string_length == 1) {
					return SUCCESS;
				}
			} else {
				return SUCCESS;
			}
			if (zend_hash_has_more_elements_ex(aht, &intern->pos) != SUCCESS) {
				return FAILURE;
			}
			zend_hash_move_forward_ex(aht, &intern->pos);
			spl_array_update_pos(intern);
		} while (1);
	}
	return FAILURE;
} /* }}} */