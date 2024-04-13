static PHP_INI_MH(OnUpdateErrorLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) && new_value && strcmp(ZSTR_VAL(new_value), "syslog")) {
		if (PG(open_basedir) && php_check_open_basedir(ZSTR_VAL(new_value))) {
			return FAILURE;
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}