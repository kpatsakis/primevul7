*/
PHP_FUNCTION(timezone_name_from_abbr)
{
	char    *abbr;
	char    *tzid;
	int      abbr_len;
	long     gmtoffset = -1;
	long     isdst = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &abbr, &abbr_len, &gmtoffset, &isdst) == FAILURE) {
		RETURN_FALSE;
	}
	tzid = timelib_timezone_id_from_abbr(abbr, gmtoffset, isdst);

	if (tzid) {
		RETURN_STRING(tzid, 1);
	} else {
		RETURN_FALSE;
	}