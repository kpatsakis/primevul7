int php_request_startup(void)
{
	int retval = SUCCESS;

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi() == FAILURE) {
		return FAILURE;
	}

	php_output_activate();
	sapi_activate();
	php_hash_environment();

	zend_try {
		PG(during_request_startup) = 1;
		if (PG(expose_php)) {
			sapi_add_header(SAPI_PHP_VERSION_HEADER, sizeof(SAPI_PHP_VERSION_HEADER)-1, 1);
		}
	} zend_catch {
		retval = FAILURE;
	} zend_end_try();

	return retval;
}