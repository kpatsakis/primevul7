*/
PHP_FUNCTION(timezone_version_get)
{
	const timelib_tzdb *tzdb;

	tzdb = DATE_TIMEZONEDB;
	RETURN_STRING(tzdb->version, 1);