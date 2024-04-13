PHPAPI ZEND_COLD void php_win32_docref2_from_error(DWORD error, const char *param1, const char *param2) {
	if (error == 0) {
		php_error_docref2(NULL, param1, param2, E_WARNING, "%s", strerror(errno));
	} else {
		char buf[PHP_WIN32_ERROR_MSG_BUFFER_SIZE + 1];
		int buf_len;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buf, PHP_WIN32_ERROR_MSG_BUFFER_SIZE, NULL);
		buf_len = (int)strlen(buf);
		if (buf_len >= 2) {
			buf[buf_len - 1] = '\0';
			buf[buf_len - 2] = '\0';
		}
		php_error_docref2(NULL, param1, param2, E_WARNING, "%s (code: %lu)", (char *)buf, error);
	}
}