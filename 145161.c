static int php_stream_open_for_zend(const char *filename, zend_file_handle *handle) /* {{{ */
{
	return php_stream_open_for_zend_ex(filename, handle, USE_PATH|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE);
}