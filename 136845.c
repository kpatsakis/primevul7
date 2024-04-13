PHP_METHOD(Phar, setDefaultStub)
{
	char *index = NULL, *webindex = NULL, *error = NULL;
	zend_string *stub = NULL;
	size_t index_len = 0, webindex_len = 0;
	int created_stub = 0;
	PHAR_ARCHIVE_OBJECT();

	if (phar_obj->archive->is_data) {
		if (phar_obj->archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"A Phar stub cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"A Phar stub cannot be set in a plain zip archive");
		}
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!s", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() > 0 && (phar_obj->archive->is_tar || phar_obj->archive->is_zip)) {
		php_error_docref(NULL, E_WARNING, "method accepts no arguments for a tar- or zip-based phar stub, %d given", ZEND_NUM_ARGS());
		RETURN_FALSE;
	}

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot change stub: phar.readonly=1");
		RETURN_FALSE;
	}

	if (!phar_obj->archive->is_tar && !phar_obj->archive->is_zip) {
		stub = phar_create_default_stub(index, webindex, &error);

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "%s", error);
			efree(error);
			if (stub) {
				zend_string_free(stub);
			}
			RETURN_FALSE;
		}

		created_stub = 1;
	}

	if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
		return;
	}
	phar_flush(phar_obj->archive, stub ? ZSTR_VAL(stub) : 0, stub ? ZSTR_LEN(stub) : 0, 1, &error);

	if (created_stub) {
		zend_string_free(stub);
	}

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}