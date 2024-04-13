static void sapi_globals_dtor(sapi_globals_struct *sapi_globals TSRMLS_DC)
{
	zend_hash_destroy(&sapi_globals->known_post_content_types);
}