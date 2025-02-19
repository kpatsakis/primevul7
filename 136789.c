PHP_METHOD(Phar, count)
{
	/* mode can be ignored, maximum depth is 1 */
	zend_long mode;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &mode) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_hash_num_elements(&phar_obj->archive->manifest));
}