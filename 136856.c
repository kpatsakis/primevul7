PHP_METHOD(Phar, setStub)
{
	zval *zstub;
	char *stub, *error;
	size_t stub_len;
	zend_long len = -1;
	php_stream *stream;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot change stub, phar is read-only");
		return;
	}

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

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "r|l", &zstub, &len) == SUCCESS) {
		if ((php_stream_from_zval_no_verify(stream, zstub)) != NULL) {
			if (len > 0) {
				len = -len;
			} else {
				len = -1;
			}
			if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
				return;
			}
			phar_flush(phar_obj->archive, (char *) zstub, len, 0, &error);
			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
				efree(error);
			}
			RETURN_TRUE;
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Cannot change stub, unable to read from input stream");
		}
	} else if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &stub, &stub_len) == SUCCESS) {
		if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
			return;
		}
		phar_flush(phar_obj->archive, stub, stub_len, 0, &error);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
		}

		RETURN_TRUE;
	}

	RETURN_FALSE;
}