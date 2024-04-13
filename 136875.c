PHP_METHOD(Phar, setMetadata)
{
	char *error;
	zval *metadata;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &metadata) == FAILURE) {
		return;
	}

	if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
		return;
	}
	if (Z_TYPE(phar_obj->archive->metadata) != IS_UNDEF) {
		zval_ptr_dtor(&phar_obj->archive->metadata);
		ZVAL_UNDEF(&phar_obj->archive->metadata);
	}

	ZVAL_COPY(&phar_obj->archive->metadata, metadata);
	phar_obj->archive->is_modified = 1;
	phar_flush(phar_obj->archive, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
}