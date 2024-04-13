static int php_start_sapi(void)
{
	int retval = SUCCESS;

	if(!SG(sapi_started)) {
		zend_try {
			PG(during_request_startup) = 1;

			/* initialize global variables */
			PG(modules_activated) = 0;
			PG(header_is_being_sent) = 0;
			PG(connection_status) = PHP_CONNECTION_NORMAL;

			zend_activate();
			zend_set_timeout(EG(timeout_seconds), 1);
			zend_activate_modules();
			PG(modules_activated)=1;
		} zend_catch {
			retval = FAILURE;
		} zend_end_try();

		SG(sapi_started) = 1;
	}
	return retval;
}