
static int timezone_initialize(timelib_tzinfo **tzi, /*const*/ char *tz TSRMLS_DC)
{
	char *tzid;
	
	*tzi = NULL;
	
	if ((tzid = timelib_timezone_id_from_abbr(tz, -1, 0))) {
		*tzi = php_date_parse_tzfile(tzid, DATE_TIMEZONEDB TSRMLS_CC);
	} else {
		*tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	}
	
	if (*tzi) {
		return SUCCESS;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown or bad timezone (%s)", tz);
		return FAILURE;
	}