   Returns time of sunrise for a given day and location */
PHP_FUNCTION(date_sunrise)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);