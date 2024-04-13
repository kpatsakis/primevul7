static void php_zend_stream_closer(void *handle) /* {{{ */
{
	php_stream_close((php_stream*)handle);
}