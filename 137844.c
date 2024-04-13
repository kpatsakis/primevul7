
PHPAPI void php_spl_object_hash(zval *obj, char *result TSRMLS_DC) /* {{{*/
{
	intptr_t hash_handle, hash_handlers;
	char *hex;

	if (!SPL_G(hash_mask_init)) {
		if (!BG(mt_rand_is_seeded)) {
			php_mt_srand(GENERATE_SEED() TSRMLS_CC);
		}

		SPL_G(hash_mask_handle)   = (intptr_t)(php_mt_rand(TSRMLS_C) >> 1);
		SPL_G(hash_mask_handlers) = (intptr_t)(php_mt_rand(TSRMLS_C) >> 1);
		SPL_G(hash_mask_init) = 1;
	}

	hash_handle   = SPL_G(hash_mask_handle)^(intptr_t)Z_OBJ_HANDLE_P(obj);
	hash_handlers = SPL_G(hash_mask_handlers)^(intptr_t)Z_OBJ_HT_P(obj);

	spprintf(&hex, 32, "%016lx%016lx", hash_handle, hash_handlers);

	strlcpy(result, hex, 33);
	efree(hex);