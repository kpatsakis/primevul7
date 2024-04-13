
static HashTable *date_object_get_properties(zval *object TSRMLS_DC)
{
	HashTable *props;
	zval *zv;
	php_date_obj     *dateobj;


	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	props = zend_std_get_properties(object TSRMLS_CC);

	if (!dateobj->time || GC_G(gc_active)) {
		return props;
	}

	/* first we add the date and time in ISO format */
	MAKE_STD_ZVAL(zv);
	ZVAL_STRING(zv, date_format("Y-m-d H:i:s.u", 14, dateobj->time, 1), 0);
	zend_hash_update(props, "date", 5, &zv, sizeof(zv), NULL);

	/* then we add the timezone name (or similar) */
	if (dateobj->time->is_localtime) {
		MAKE_STD_ZVAL(zv);
		ZVAL_LONG(zv, dateobj->time->zone_type);
		zend_hash_update(props, "timezone_type", 14, &zv, sizeof(zv), NULL);

		MAKE_STD_ZVAL(zv);
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				ZVAL_STRING(zv, dateobj->time->tz_info->name, 1);
				break;
			case TIMELIB_ZONETYPE_OFFSET: {
				char *tmpstr = emalloc(sizeof("UTC+05:00"));
				timelib_sll utc_offset = dateobj->time->z;

				snprintf(tmpstr, sizeof("+05:00"), "%c%02d:%02d",
					utc_offset > 0 ? '-' : '+',
					abs(utc_offset / 60),
					abs((utc_offset % 60)));

				ZVAL_STRING(zv, tmpstr, 0);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				ZVAL_STRING(zv, dateobj->time->tz_abbr, 1);
				break;
		}
		zend_hash_update(props, "timezone", 9, &zv, sizeof(zv), NULL);
	}

	return props;