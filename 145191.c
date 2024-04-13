PHPAPI ZEND_COLD void php_log_err(char *log_message)
{
	int fd = -1;
	time_t error_time;

	if (PG(in_error_log)) {
		/* prevent recursive invocation */
		return;
	}
	PG(in_error_log) = 1;

	/* Try to use the specified logging location. */
	if (PG(error_log) != NULL) {
#ifdef HAVE_SYSLOG_H
		if (!strcmp(PG(error_log), "syslog")) {
			php_syslog(LOG_NOTICE, "%s", log_message);
			PG(in_error_log) = 0;
			return;
		}
#endif
		fd = VCWD_OPEN_MODE(PG(error_log), O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (fd != -1) {
			char *tmp;
			size_t len;
			zend_string *error_time_str;

			time(&error_time);
#ifdef ZTS
			if (!php_during_module_startup()) {
				error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 1);
			} else {
				error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 0);
			}
#else
			error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 1);
#endif
			len = spprintf(&tmp, 0, "[%s] %s%s", ZSTR_VAL(error_time_str), log_message, PHP_EOL);
#ifdef PHP_WIN32
			php_flock(fd, 2);
			/* XXX should eventually write in a loop if len > UINT_MAX */
			php_ignore_value(write(fd, tmp, (unsigned)len));
#else
			php_ignore_value(write(fd, tmp, len));
#endif
			efree(tmp);
			zend_string_free(error_time_str);
			close(fd);
			PG(in_error_log) = 0;
			return;
		}
	}

	/* Otherwise fall back to the default logging location, if we have one */

	if (sapi_module.log_message) {
		sapi_module.log_message(log_message);
	}
	PG(in_error_log) = 0;
}