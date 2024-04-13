static FILE *php_fopen_wrapper_for_zend(const char *filename, zend_string **opened_path)
{
	return php_stream_open_wrapper_as_file((char *)filename, "rb", USE_PATH|IGNORE_URL_WIN|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE, opened_path);
}