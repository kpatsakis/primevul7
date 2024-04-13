static ZEND_COLD void php_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	char *buffer;
	int buffer_len, display;

	buffer_len = (int)vspprintf(&buffer, PG(log_errors_max_len), format, args);

	/* check for repeated errors to be ignored */
	if (PG(ignore_repeated_errors) && PG(last_error_message)) {
		/* no check for PG(last_error_file) is needed since it cannot
		 * be NULL if PG(last_error_message) is not NULL */
		if (strcmp(PG(last_error_message), buffer)
			|| (!PG(ignore_repeated_source)
				&& ((PG(last_error_lineno) != (int)error_lineno)
					|| strcmp(PG(last_error_file), error_filename)))) {
			display = 1;
		} else {
			display = 0;
		}
	} else {
		display = 1;
	}

	/* store the error if it has changed */
	if (display) {
		if (PG(last_error_message)) {
			char *s = PG(last_error_message);
			PG(last_error_message) = NULL;
			free(s);
		}
		if (PG(last_error_file)) {
			char *s = PG(last_error_file);
			PG(last_error_file) = NULL;
			free(s);
		}
		if (!error_filename) {
			error_filename = "Unknown";
		}
		PG(last_error_type) = type;
		PG(last_error_message) = strdup(buffer);
		PG(last_error_file) = strdup(error_filename);
		PG(last_error_lineno) = error_lineno;
	}

	/* according to error handling mode, suppress error, throw exception or show it */
	if (EG(error_handling) != EH_NORMAL) {
		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
				/* fatal errors are real errors and cannot be made exceptions */
				break;
			case E_STRICT:
			case E_DEPRECATED:
			case E_USER_DEPRECATED:
				/* for the sake of BC to old damaged code */
				break;
			case E_NOTICE:
			case E_USER_NOTICE:
				/* notices are no errors and are not treated as such like E_WARNINGS */
				break;
			default:
				/* throw an exception if we are in EH_THROW mode
				 * but DO NOT overwrite a pending exception
				 */
				if (EG(error_handling) == EH_THROW && !EG(exception)) {
					zend_throw_error_exception(EG(exception_class), buffer, 0, type);
				}
				efree(buffer);
				return;
		}
	}

	/* display/log the error if necessary */
	if (display && (EG(error_reporting) & type || (type & E_CORE))
		&& (PG(log_errors) || PG(display_errors) || (!module_initialized))) {
		char *error_type_str;

		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
				error_type_str = "Fatal error";
				break;
			case E_RECOVERABLE_ERROR:
				error_type_str = "Catchable fatal error";
				break;
			case E_WARNING:
			case E_CORE_WARNING:
			case E_COMPILE_WARNING:
			case E_USER_WARNING:
				error_type_str = "Warning";
				break;
			case E_PARSE:
				error_type_str = "Parse error";
				break;
			case E_NOTICE:
			case E_USER_NOTICE:
				error_type_str = "Notice";
				break;
			case E_STRICT:
				error_type_str = "Strict Standards";
				break;
			case E_DEPRECATED:
			case E_USER_DEPRECATED:
				error_type_str = "Deprecated";
				break;
			default:
				error_type_str = "Unknown error";
				break;
		}

		if (!module_initialized || PG(log_errors)) {
			char *log_buffer;
#ifdef PHP_WIN32
			if (type == E_CORE_ERROR || type == E_CORE_WARNING) {
				syslog(LOG_ALERT, "PHP %s: %s (%s)", error_type_str, buffer, GetCommandLine());
			}
#endif
			spprintf(&log_buffer, 0, "PHP %s:  %s in %s on line %d", error_type_str, buffer, error_filename, error_lineno);
			php_log_err(log_buffer);
			efree(log_buffer);
		}

		if (PG(display_errors) && ((module_initialized && !PG(during_request_startup)) || (PG(display_startup_errors)))) {
			if (PG(xmlrpc_errors)) {
				php_printf("<?xml version=\"1.0\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%pd</int></value></member><member><name>faultString</name><value><string>%s:%s in %s on line %d</string></value></member></struct></value></fault></methodResponse>", PG(xmlrpc_error_number), error_type_str, buffer, error_filename, error_lineno);
			} else {
				char *prepend_string = INI_STR("error_prepend_string");
				char *append_string = INI_STR("error_append_string");

				if (PG(html_errors)) {
					if (type == E_ERROR || type == E_PARSE) {
						zend_string *buf = php_escape_html_entities((unsigned char*)buffer, buffer_len, 0, ENT_COMPAT, NULL);
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, ZSTR_VAL(buf), error_filename, error_lineno, STR_PRINT(append_string));
						zend_string_free(buf);
					} else {
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
					}
				} else {
					/* Write CLI/CGI errors to stderr if display_errors = "stderr" */
					if ((!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi")) &&
						PG(display_errors) == PHP_DISPLAY_ERRORS_STDERR
					) {
#ifdef PHP_WIN32
						fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
						fflush(stderr);
#else
						fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
#endif
					} else {
						php_printf("%s\n%s: %s in %s on line %d\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
					}
				}
			}
		}
#if ZEND_DEBUG
		if (PG(report_zend_debug)) {
			zend_bool trigger_break;

			switch (type) {
				case E_ERROR:
				case E_CORE_ERROR:
				case E_COMPILE_ERROR:
				case E_USER_ERROR:
					trigger_break=1;
					break;
				default:
					trigger_break=0;
					break;
			}
			zend_output_debug_string(trigger_break, "%s(%d) : %s - %s", error_filename, error_lineno, error_type_str, buffer);
		}
#endif
	}

	/* Bail out if we can't recover */
	switch (type) {
		case E_CORE_ERROR:
			if(!module_initialized) {
				/* bad error in module startup - no way we can live with this */
				exit(-2);
			}
		/* no break - intentionally */
		case E_ERROR:
		case E_RECOVERABLE_ERROR:
		case E_PARSE:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
			EG(exit_status) = 255;
			if (module_initialized) {
				if (!PG(display_errors) &&
				    !SG(headers_sent) &&
					SG(sapi_headers).http_response_code == 200
				) {
					sapi_header_line ctr = {0};

					ctr.line = "HTTP/1.0 500 Internal Server Error";
					ctr.line_len = sizeof("HTTP/1.0 500 Internal Server Error") - 1;
					sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
				}
				/* the parser would return 1 (failure), we can bail out nicely */
				if (type == E_PARSE) {
					CG(parse_error) = 0;
				} else {
					/* restore memory limit */
					zend_set_memory_limit(PG(memory_limit));
					efree(buffer);
					zend_objects_store_mark_destructed(&EG(objects_store));
					zend_bailout();
					return;
				}
			}
			break;
	}

	/* Log if necessary */
	if (!display) {
		efree(buffer);
		return;
	}

	if (PG(track_errors) && module_initialized && EG(valid_symbol_table)) {
		zval tmp;

		ZVAL_STRINGL(&tmp, buffer, buffer_len);
		if (EG(current_execute_data)) {
			if (zend_set_local_var_str("php_errormsg", sizeof("php_errormsg")-1, &tmp, 0) == FAILURE) {
				zval_ptr_dtor(&tmp);
			}
		} else {
			zend_hash_str_update_ind(&EG(symbol_table), "php_errormsg", sizeof("php_errormsg")-1, &tmp);
		}
	}

	efree(buffer);
}