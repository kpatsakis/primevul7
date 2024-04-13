PHP_METHOD(Phar, isFileFormat)
{
	zend_long type;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &type) == FAILURE) {
		RETURN_FALSE;
	}

	switch (type) {
		case PHAR_FORMAT_TAR:
			RETURN_BOOL(phar_obj->archive->is_tar);
		case PHAR_FORMAT_ZIP:
			RETURN_BOOL(phar_obj->archive->is_zip);
		case PHAR_FORMAT_PHAR:
			RETURN_BOOL(!phar_obj->archive->is_tar && !phar_obj->archive->is_zip);
		default:
			zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown file format specified");
	}
}