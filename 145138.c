PHPAPI int php_execute_simple_script(zend_file_handle *primary_file, zval *ret)
{
	char *old_cwd;
	ALLOCA_FLAG(use_heap)

	EG(exit_status) = 0;
#define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE, use_heap);
	old_cwd[0] = '\0';

	zend_try {
#ifdef PHP_WIN32
		if(primary_file->filename) {
			UpdateIniFromRegistry((char*)primary_file->filename);
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
			VCWD_CHDIR_FILE(primary_file->filename);
		}
		zend_execute_scripts(ZEND_REQUIRE, ret, 1, primary_file);
	} zend_end_try();

	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}

	free_alloca(old_cwd, use_heap);
	return EG(exit_status);
}