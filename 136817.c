PHP_METHOD(Phar, delMetadata)
{
	char *error;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (Z_TYPE(phar_obj->archive->metadata) != IS_UNDEF) {
		zval_ptr_dtor(&phar_obj->archive->metadata);
		ZVAL_UNDEF(&phar_obj->archive->metadata);
		phar_obj->archive->is_modified = 1;
		phar_flush(phar_obj->archive, 0, 0, 0, &error);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

	} else {
		RETURN_TRUE;
	}
}