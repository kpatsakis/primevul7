PHP_METHOD(Phar, getSupportedCompression)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	phar_request_initialize();

	if (PHAR_G(has_zlib)) {
		add_next_index_stringl(return_value, "GZ", 2);
	}

	if (PHAR_G(has_bz2)) {
		add_next_index_stringl(return_value, "BZIP2", 5);
	}
}