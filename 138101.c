*/
PHP_FUNCTION(date_timezone_get)
{
	zval             *object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime/* && dateobj->time->tz_info*/) {
		php_date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
		tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
		tzobj->initialized = 1;
		tzobj->type = dateobj->time->zone_type;
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				tzobj->tzi.tz = dateobj->time->tz_info;
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				tzobj->tzi.utc_offset = dateobj->time->z;
				break;
			case TIMELIB_ZONETYPE_ABBR:
				tzobj->tzi.z.utc_offset = dateobj->time->z;
				tzobj->tzi.z.dst = dateobj->time->dst;
				tzobj->tzi.z.abbr = strdup(dateobj->time->tz_abbr);
				break;
		}
	} else {
		RETURN_FALSE;
	}