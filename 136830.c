PHP_METHOD(Phar, stopBuffering)
{
	char *error;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	phar_obj->archive->donotflush = 0;
	phar_flush(phar_obj->archive, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
}