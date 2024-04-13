PHPAPI int php_execute_script(zend_file_handle *primary_file)
{
	zend_file_handle *prepend_file_p, *append_file_p;
	zend_file_handle prepend_file = {{0}, NULL, NULL, 0, 0}, append_file = {{0}, NULL, NULL, 0, 0};
#if HAVE_BROKEN_GETCWD
	volatile int old_cwd_fd = -1;
#else
	char *old_cwd;
	ALLOCA_FLAG(use_heap)
#endif
	int retval = 0;

	EG(exit_status) = 0;
#ifndef HAVE_BROKEN_GETCWD
# define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE, use_heap);
	old_cwd[0] = '\0';
#endif

	zend_try {
		char realfile[MAXPATHLEN];

#ifdef PHP_WIN32
		if(primary_file->filename) {
			UpdateIniFromRegistry((char*)primary_file->filename);
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
#if HAVE_BROKEN_GETCWD
			/* this looks nasty to me */
			old_cwd_fd = open(".", 0);
#else
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
#endif
			VCWD_CHDIR_FILE(primary_file->filename);
		}

 		/* Only lookup the real file path and add it to the included_files list if already opened
		 *   otherwise it will get opened and added to the included_files list in zend_execute_scripts
		 */
 		if (primary_file->filename &&
 		    (primary_file->filename[0] != '-' || primary_file->filename[1] != 0) &&
 			primary_file->opened_path == NULL &&
 			primary_file->type != ZEND_HANDLE_FILENAME
		) {
			if (expand_filepath(primary_file->filename, realfile)) {
				primary_file->opened_path = zend_string_init(realfile, strlen(realfile), 0);
				zend_hash_add_empty_element(&EG(included_files), primary_file->opened_path);
			}
		}

		if (PG(auto_prepend_file) && PG(auto_prepend_file)[0]) {
			prepend_file.filename = PG(auto_prepend_file);
			prepend_file.opened_path = NULL;
			prepend_file.free_filename = 0;
			prepend_file.type = ZEND_HANDLE_FILENAME;
			prepend_file_p = &prepend_file;
		} else {
			prepend_file_p = NULL;
		}

		if (PG(auto_append_file) && PG(auto_append_file)[0]) {
			append_file.filename = PG(auto_append_file);
			append_file.opened_path = NULL;
			append_file.free_filename = 0;
			append_file.type = ZEND_HANDLE_FILENAME;
			append_file_p = &append_file;
		} else {
			append_file_p = NULL;
		}
		if (PG(max_input_time) != -1) {
#ifdef PHP_WIN32
			zend_unset_timeout();
#endif
			zend_set_timeout(INI_INT("max_execution_time"), 0);
		}

		/*
		   If cli primary file has shabang line and there is a prepend file,
		   the `start_lineno` will be used by prepend file but not primary file,
		   save it and restore after prepend file been executed.
		 */
		if (CG(start_lineno) && prepend_file_p) {
			int orig_start_lineno = CG(start_lineno);

			CG(start_lineno) = 0;
			if (zend_execute_scripts(ZEND_REQUIRE, NULL, 1, prepend_file_p) == SUCCESS) {
				CG(start_lineno) = orig_start_lineno;
				retval = (zend_execute_scripts(ZEND_REQUIRE, NULL, 2, primary_file, append_file_p) == SUCCESS);
			}
		} else {
			retval = (zend_execute_scripts(ZEND_REQUIRE, NULL, 3, prepend_file_p, primary_file, append_file_p) == SUCCESS);
		}
	} zend_end_try();

	if (EG(exception)) {
		zend_try {
			zend_exception_error(EG(exception), E_ERROR);
		} zend_end_try();
	}

#if HAVE_BROKEN_GETCWD
	if (old_cwd_fd != -1) {
		fchdir(old_cwd_fd);
		close(old_cwd_fd);
	}
#else
	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}
	free_alloca(old_cwd, use_heap);
#endif
	return retval;
}