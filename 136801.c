PHP_METHOD(Phar, startBuffering)
{
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	phar_obj->archive->donotflush = 1;
}