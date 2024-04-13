PHP_METHOD(Phar, canCompress)
{
	zend_long method = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &method) == FAILURE) {
		return;
	}

	phar_request_initialize();
	switch (method) {
	case PHAR_ENT_COMPRESSED_GZ:
		if (PHAR_G(has_zlib)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	case PHAR_ENT_COMPRESSED_BZ2:
		if (PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	default:
		if (PHAR_G(has_zlib) || PHAR_G(has_bz2)) {
			RETURN_TRUE;
		} else {
			RETURN_FALSE;
		}
	}
}