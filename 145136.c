int php_request_startup_for_hook(void)
{
	int retval = SUCCESS;

#if PHP_SIGCHLD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi() == FAILURE) {
		return FAILURE;
	}

	php_output_activate();
	sapi_activate_headers_only();
	php_hash_environment();

	return retval;
}