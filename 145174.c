void php_on_timeout(int seconds)
{
	PG(connection_status) |= PHP_CONNECTION_TIMEOUT;
	zend_set_timeout(EG(timeout_seconds), 1);
	if(PG(exit_on_timeout)) sapi_terminate_process();
}