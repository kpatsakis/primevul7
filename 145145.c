static size_t php_zend_stream_fsizer(void *handle) /* {{{ */
{
	php_stream_statbuf  ssb;
	if (php_stream_stat((php_stream*)handle, &ssb) == 0) {
		return ssb.sb.st_size;
	}
	return 0;
}