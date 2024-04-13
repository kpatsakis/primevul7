PHPAPI int php_hash_environment(TSRMLS_D)
{
	memset(PG(http_globals), 0, sizeof(PG(http_globals)));
	zend_activate_auto_globals(TSRMLS_C);
	if (PG(register_argc_argv)) {
		php_build_argv(SG(request_info).query_string, PG(http_globals)[TRACK_VARS_SERVER] TSRMLS_CC);
	}
	return SUCCESS;
}