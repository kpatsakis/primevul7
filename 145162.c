static void php_zend_stream_mmap_closer(void *handle) /* {{{ */
{
	php_stream_mmap_unmap((php_stream*)handle);
	php_zend_stream_closer(handle);
}