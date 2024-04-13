static void sapi_globals_ctor(sapi_globals_struct *sapi_globals TSRMLS_DC)
{
	memset(sapi_globals, 0, sizeof(*sapi_globals));
	zend_hash_init_ex(&sapi_globals->known_post_content_types, 5, NULL, NULL, 1, 0);
	php_setup_sapi_content_types(TSRMLS_C);
}