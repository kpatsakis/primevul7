void php_startup_auto_globals(TSRMLS_D)
{
	zend_register_auto_global(ZEND_STRL("_GET"), 0, php_auto_globals_create_get TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_POST"), 0, php_auto_globals_create_post TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_COOKIE"), 0, php_auto_globals_create_cookie TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_SERVER"), PG(auto_globals_jit), php_auto_globals_create_server TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_ENV"), PG(auto_globals_jit), php_auto_globals_create_env TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_REQUEST"), PG(auto_globals_jit), php_auto_globals_create_request TSRMLS_CC);
	zend_register_auto_global(ZEND_STRL("_FILES"), 0, php_auto_globals_create_files TSRMLS_CC);
}