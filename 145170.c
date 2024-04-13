static ZEND_COLD void php_message_handler_for_zend(zend_long message, const void *data)
{
	switch (message) {
		case ZMSG_FAILED_INCLUDE_FOPEN:
			php_error_docref("function.include", E_WARNING, "Failed opening '%s' for inclusion (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_REQUIRE_FOPEN:
			php_error_docref("function.require", E_COMPILE_ERROR, "Failed opening required '%s' (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_HIGHLIGHT_FOPEN:
			php_error_docref(NULL, E_WARNING, "Failed opening '%s' for highlighting", php_strip_url_passwd((char *) data));
			break;
		case ZMSG_MEMORY_LEAK_DETECTED:
		case ZMSG_MEMORY_LEAK_REPEATED:
#if ZEND_DEBUG
			if (EG(error_reporting) & E_WARNING) {
				char memory_leak_buf[1024];

				if (message==ZMSG_MEMORY_LEAK_DETECTED) {
					zend_leak_info *t = (zend_leak_info *) data;

					snprintf(memory_leak_buf, 512, "%s(%d) :  Freeing 0x%.8lX (%zu bytes), script=%s\n", t->filename, t->lineno, (zend_uintptr_t)t->addr, t->size, SAFE_FILENAME(SG(request_info).path_translated));
					if (t->orig_filename) {
						char relay_buf[512];

						snprintf(relay_buf, 512, "%s(%d) : Actual location (location was relayed)\n", t->orig_filename, t->orig_lineno);
						strlcat(memory_leak_buf, relay_buf, sizeof(memory_leak_buf));
					}
				} else {
					unsigned long leak_count = (zend_uintptr_t) data;

					snprintf(memory_leak_buf, 512, "Last leak repeated %ld time%s\n", leak_count, (leak_count>1?"s":""));
				}
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
#endif
			break;
		case ZMSG_MEMORY_LEAKS_GRAND_TOTAL:
#if ZEND_DEBUG
			if (EG(error_reporting) & E_WARNING) {
				char memory_leak_buf[512];

				snprintf(memory_leak_buf, 512, "=== Total %d memory leaks detected ===\n", *((uint32_t *) data));
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
#endif
			break;
		case ZMSG_LOG_SCRIPT_NAME: {
				struct tm *ta, tmbuf;
				time_t curtime;
				char *datetime_str, asctimebuf[52];
				char memory_leak_buf[4096];

				time(&curtime);
				ta = php_localtime_r(&curtime, &tmbuf);
				datetime_str = php_asctime_r(ta, asctimebuf);
				if (datetime_str) {
					datetime_str[strlen(datetime_str)-1]=0;	/* get rid of the trailing newline */
					snprintf(memory_leak_buf, sizeof(memory_leak_buf), "[%s]  Script:  '%s'\n", datetime_str, SAFE_FILENAME(SG(request_info).path_translated));
				} else {
					snprintf(memory_leak_buf, sizeof(memory_leak_buf), "[null]  Script:  '%s'\n", SAFE_FILENAME(SG(request_info).path_translated));
				}
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
			break;
	}
}