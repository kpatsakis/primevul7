
timelib_tzinfo *php_date_parse_tzfile_wrapper(char *formal_tzname, const timelib_tzdb *tzdb)
{
	TSRMLS_FETCH();
	return php_date_parse_tzfile(formal_tzname, tzdb TSRMLS_CC);