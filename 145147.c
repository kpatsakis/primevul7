PHPAPI int php_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode) /* {{{ */
{
	char *p;
	size_t len, mapped_len;
	php_stream *stream = php_stream_open_wrapper((char *)filename, "rb", mode, &handle->opened_path);

	if (stream) {
#if HAVE_MMAP || defined(PHP_WIN32)
		size_t page_size = REAL_PAGE_SIZE;
#endif

		handle->filename = (char*)filename;
		handle->free_filename = 0;
		handle->handle.stream.handle  = stream;
		handle->handle.stream.reader  = (zend_stream_reader_t)_php_stream_read;
		handle->handle.stream.fsizer  = php_zend_stream_fsizer;
		handle->handle.stream.isatty  = 0;
		/* can we mmap immediately? */
		memset(&handle->handle.stream.mmap, 0, sizeof(handle->handle.stream.mmap));
		len = php_zend_stream_fsizer(stream);
		if (len != 0
#if HAVE_MMAP || defined(PHP_WIN32)
		&& ((len - 1) % page_size) <= page_size - ZEND_MMAP_AHEAD
#endif
		&& php_stream_mmap_possible(stream)
		&& (p = php_stream_mmap_range(stream, 0, len, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped_len)) != NULL) {
			handle->handle.stream.closer   = php_zend_stream_mmap_closer;
			handle->handle.stream.mmap.buf = p;
			handle->handle.stream.mmap.len = mapped_len;
			handle->type = ZEND_HANDLE_MAPPED;
		} else {
			handle->handle.stream.closer = php_zend_stream_closer;
			handle->type = ZEND_HANDLE_STREAM;
		}
		/* suppress warning if this stream is not explicitly closed */
		php_stream_auto_cleanup(stream);

		return SUCCESS;
	}
	return FAILURE;
}