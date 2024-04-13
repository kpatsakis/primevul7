PHP_FUNCTION(exif_tagname)
{
	long tag;
	char *szTemp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &tag) == FAILURE) {
		return;
	}

	szTemp = exif_get_tagname(tag, NULL, 0, tag_table_IFD TSRMLS_CC);

	if (tag < 0 || !szTemp || !szTemp[0]) {
		RETURN_FALSE;
	}

	RETURN_STRING(szTemp, 1)
}