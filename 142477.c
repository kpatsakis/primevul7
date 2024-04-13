PHP_FUNCTION(symlink)
{
	char *topath, *frompath;
	size_t topath_len, frompath_len;
	BOOLEAN ret;
	char source_p[MAXPATHLEN];
	char dest_p[MAXPATHLEN];
	char dirname[MAXPATHLEN];
	size_t len;
	DWORD attr;
	wchar_t *dstw, *srcw;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "pp", &topath, &topath_len, &frompath, &frompath_len) == FAILURE) {
		return;
	}

	if (!expand_filepath(frompath, source_p)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	memcpy(dirname, source_p, sizeof(source_p));
	len = php_dirname(dirname, strlen(dirname));

	if (!expand_filepath_ex(topath, dest_p, dirname, len)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	if (php_stream_locate_url_wrapper(source_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY) ||
		php_stream_locate_url_wrapper(dest_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY) )
	{
		php_error_docref(NULL, E_WARNING, "Unable to symlink to a URL");
		RETURN_FALSE;
	}

	if (OPENBASEDIR_CHECKPATH(dest_p)) {
		RETURN_FALSE;
	}

	if (OPENBASEDIR_CHECKPATH(source_p)) {
		RETURN_FALSE;
	}

	dstw = php_win32_ioutil_any_to_w(topath);
	if (!dstw) {
		php_error_docref(NULL, E_WARNING, "UTF-16 conversion failed (error %d)", GetLastError());
		RETURN_FALSE;
	}
	if ((attr = GetFileAttributesW(dstw)) == INVALID_FILE_ATTRIBUTES) {
		free(dstw);
		php_error_docref(NULL, E_WARNING, "Could not fetch file information(error %d)", GetLastError());
		RETURN_FALSE;
	}

	srcw = php_win32_ioutil_any_to_w(source_p);
	if (!srcw) {
		free(dstw);
		php_error_docref(NULL, E_WARNING, "UTF-16 conversion failed (error %d)", GetLastError());
		RETURN_FALSE;
	}
	/* For the source, an expanded path must be used (in ZTS an other thread could have changed the CWD).
	 * For the target the exact string given by the user must be used, relative or not, existing or not.
	 * The target is relative to the link itself, not to the CWD. */
	ret = CreateSymbolicLinkW(srcw, dstw, (attr & FILE_ATTRIBUTE_DIRECTORY ? 1 : 0));

	if (!ret) {
		free(dstw);
		free(srcw);
		php_error_docref(NULL, E_WARNING, "Cannot create symlink, error code(%d)", GetLastError());
		RETURN_FALSE;
	}

	free(dstw);
	free(srcw);

	RETURN_TRUE;
}