
static int date_object_compare_date(zval *d1, zval *d2 TSRMLS_DC)
{
	if (Z_TYPE_P(d1) == IS_OBJECT && Z_TYPE_P(d2) == IS_OBJECT &&
		instanceof_function(Z_OBJCE_P(d1), date_ce_date TSRMLS_CC) &&
		instanceof_function(Z_OBJCE_P(d2), date_ce_date TSRMLS_CC)) {
		php_date_obj *o1 = zend_object_store_get_object(d1 TSRMLS_CC);
		php_date_obj *o2 = zend_object_store_get_object(d2 TSRMLS_CC);

		if (!o1->time || !o2->time) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to compare an incomplete DateTime object");
			return 1;
		}
		if (!o1->time->sse_uptodate) {
			timelib_update_ts(o1->time, o1->time->tz_info);
		}
		if (!o2->time->sse_uptodate) {
			timelib_update_ts(o2->time, o2->time->tz_info);
		}
		
		return (o1->time->sse == o2->time->sse) ? 0 : ((o1->time->sse < o2->time->sse) ? -1 : 1);
	}
	
	return 1;