/* {{{ static PHP_INI_MH(OnUpdate_date_timezone) */
static PHP_INI_MH(OnUpdate_date_timezone)
{
	if (OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	DATEG(timezone_valid) = 0;
	if (stage == PHP_INI_STAGE_RUNTIME) {
		if (!timelib_timezone_id_is_valid(DATEG(default_timezone), DATE_TIMEZONEDB)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, DATE_TZ_ERRMSG);
		} else {
			DATEG(timezone_valid) = 1;
		}
	}

	return SUCCESS;