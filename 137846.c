 Register and return default file extensions for spl_autoload */
PHP_FUNCTION(spl_autoload_extensions)
{
	char *file_exts = NULL;
	int file_exts_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &file_exts, &file_exts_len) == FAILURE) {
		return;
	}
	if (file_exts) {
		if (SPL_G(autoload_extensions)) {
			efree(SPL_G(autoload_extensions));
		}
		SPL_G(autoload_extensions) = estrndup(file_exts, file_exts_len);
		SPL_G(autoload_extensions_len) = file_exts_len;
	}

	if (SPL_G(autoload_extensions) == NULL) {
		RETURN_STRINGL(SPL_DEFAULT_FILE_EXTENSIONS, sizeof(SPL_DEFAULT_FILE_EXTENSIONS) - 1, 1);
	} else {
		RETURN_STRINGL(SPL_G(autoload_extensions), SPL_G(autoload_extensions_len), 1);
	}