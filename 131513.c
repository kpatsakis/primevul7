SPL_API int spl_hash_verify_pos_ex(spl_array_object * intern, HashTable * ht TSRMLS_DC) /* {{{ */
{
	Bucket *p;

/*	IS_CONSISTENT(ht);*/

/*	HASH_PROTECT_RECURSION(ht);*/
	p = ht->arBuckets[intern->pos_h & ht->nTableMask];
	while (p != NULL) {
		if (p == intern->pos) {
			return SUCCESS;
		}
		p = p->pNext;
	}
/*	HASH_UNPROTECT_RECURSION(ht); */
	spl_array_rewind(intern TSRMLS_CC);
	return FAILURE;

} /* }}} */