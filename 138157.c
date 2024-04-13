
static HashTable *date_object_get_properties_period(zval *object TSRMLS_DC)
{
	HashTable		*props;
	zval			*zv;
	php_period_obj	*period_obj;

	period_obj = zend_object_store_get_object(object TSRMLS_CC);

	props = zend_std_get_properties(object TSRMLS_CC);

	if (!period_obj->start || GC_G(gc_active)) {
		return props;
	}

	MAKE_STD_ZVAL(zv);
	if (period_obj->start) {
		php_date_obj *date_obj;
		object_init_ex(zv, date_ce_date);
		date_obj = zend_object_store_get_object(zv TSRMLS_CC);
		date_obj->time = timelib_time_clone(period_obj->start);
	} else {
		ZVAL_NULL(zv);
	}
	zend_hash_update(props, "start", sizeof("start"), &zv, sizeof(zv), NULL);

	MAKE_STD_ZVAL(zv);
	if (period_obj->current) {
		php_date_obj *date_obj;
		object_init_ex(zv, date_ce_date);
		date_obj = zend_object_store_get_object(zv TSRMLS_CC);
		date_obj->time = timelib_time_clone(period_obj->current);
	} else {
		ZVAL_NULL(zv);
	}
	zend_hash_update(props, "current", sizeof("current"), &zv, sizeof(zv), NULL);

	MAKE_STD_ZVAL(zv);
	if (period_obj->end) {
		php_date_obj *date_obj;
		object_init_ex(zv, date_ce_date);
		date_obj = zend_object_store_get_object(zv TSRMLS_CC);
		date_obj->time = timelib_time_clone(period_obj->end);
	} else {
		ZVAL_NULL(zv);
	}
	zend_hash_update(props, "end", sizeof("end"), &zv, sizeof(zv), NULL);

	MAKE_STD_ZVAL(zv);
	if (period_obj->interval) {
		php_interval_obj *interval_obj;
		object_init_ex(zv, date_ce_interval);
		interval_obj = zend_object_store_get_object(zv TSRMLS_CC);
		interval_obj->diff = timelib_rel_time_clone(period_obj->interval);
		interval_obj->initialized = 1;
	} else {
		ZVAL_NULL(zv);
	}
	zend_hash_update(props, "interval", sizeof("interval"), &zv, sizeof(zv), NULL);
	
	/* converted to larger type (int->long); must check when unserializing */
	MAKE_STD_ZVAL(zv);
	ZVAL_LONG(zv, (long) period_obj->recurrences);
	zend_hash_update(props, "recurrences", sizeof("recurrences"), &zv, sizeof(zv), NULL);

	MAKE_STD_ZVAL(zv);
	ZVAL_BOOL(zv, period_obj->include_start_date);
	zend_hash_update(props, "include_start_date", sizeof("include_start_date"), &zv, sizeof(zv), NULL);

	return props;