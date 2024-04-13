PHP_METHOD(Phar, decompress)
{
	char *ext = NULL;
	size_t ext_len = 0;
	zend_object *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot decompress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot decompress zip-based archives with whole-archive compression");
		return;
	}

	if (phar_obj->archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->archive, PHAR_FORMAT_TAR, ext, PHAR_FILE_COMPRESSED_NONE);
	} else {
		ret = phar_convert_to_other(phar_obj->archive, PHAR_FORMAT_PHAR, ext, PHAR_FILE_COMPRESSED_NONE);
	}

	if (ret) {
		ZVAL_OBJ(return_value, ret);
	} else {
		RETURN_NULL();
	}
}