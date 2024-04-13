SPL_API int spl_hash_verify_pos(spl_array_object * intern TSRMLS_DC) /* {{{ */
{
	HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	return spl_hash_verify_pos_ex(intern, ht TSRMLS_CC);
}