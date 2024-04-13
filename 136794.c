PHP_METHOD(Phar, decompressFiles)
{
	char *error;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (!pharobj_cancompress(&phar_obj->archive->manifest)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot decompress all files, some are compressed as bzip2 or gzip and cannot be decompressed");
		return;
	}

	if (phar_obj->archive->is_tar) {
		RETURN_TRUE;
	} else {
		if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
			return;
		}
		pharobj_set_compression(&phar_obj->archive->manifest, PHAR_ENT_COMPRESSED_NONE);
	}

	phar_obj->archive->is_modified = 1;
	phar_flush(phar_obj->archive, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}