*/
PHP_FUNCTION(timezone_name_get)
{
	zval             *object;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			RETURN_STRING(tzobj->tzi.tz->name, 1);
			break;
		case TIMELIB_ZONETYPE_OFFSET: {
			char *tmpstr = emalloc(sizeof("UTC+05:00"));
			timelib_sll utc_offset = tzobj->tzi.utc_offset;

			snprintf(tmpstr, sizeof("+05:00"), "%c%02d:%02d",
				utc_offset > 0 ? '-' : '+',
				abs(utc_offset / 60),
				abs((utc_offset % 60)));

			RETURN_STRING(tmpstr, 0);
			}
			break;
		case TIMELIB_ZONETYPE_ABBR:
			RETURN_STRING(tzobj->tzi.z.abbr, 1);
			break;
	}