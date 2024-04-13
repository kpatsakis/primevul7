PHPAPI ZEND_COLD void php_verror(const char *docref, const char *params, int type, const char *format, va_list args)
{
	zend_string *replace_buffer = NULL, *replace_origin = NULL;
	char *buffer = NULL, *docref_buf = NULL, *target = NULL;
	char *docref_target = "", *docref_root = "";
	char *p;
	int buffer_len = 0;
	const char *space = "";
	const char *class_name = "";
	const char *function;
	int origin_len;
	char *origin;
	char *message;
	int is_function = 0;

	/* get error text into buffer and escape for html if necessary */
	buffer_len = (int)vspprintf(&buffer, 0, format, args);

	if (PG(html_errors)) {
		replace_buffer = php_escape_html_entities((unsigned char*)buffer, buffer_len, 0, ENT_COMPAT, NULL);
		/* Retry with substituting invalid chars on fail. */
		if (!replace_buffer) {
			replace_buffer = php_escape_html_entities((unsigned char*)buffer, buffer_len, 0, ENT_COMPAT | ENT_HTML_SUBSTITUTE_ERRORS, NULL);
		}

		efree(buffer);

		if (replace_buffer) {
			buffer = ZSTR_VAL(replace_buffer);
			buffer_len = (int)ZSTR_LEN(replace_buffer);
		} else {
			buffer = "";
			buffer_len = 0;
		}
	}

	/* which function caused the problem if any at all */
	if (php_during_module_startup()) {
		function = "PHP Startup";
	} else if (php_during_module_shutdown()) {
		function = "PHP Shutdown";
	} else if (EG(current_execute_data) &&
				EG(current_execute_data)->func &&
				ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
				EG(current_execute_data)->opline &&
				EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL
	) {
		switch (EG(current_execute_data)->opline->extended_value) {
			case ZEND_EVAL:
				function = "eval";
				is_function = 1;
				break;
			case ZEND_INCLUDE:
				function = "include";
				is_function = 1;
				break;
			case ZEND_INCLUDE_ONCE:
				function = "include_once";
				is_function = 1;
				break;
			case ZEND_REQUIRE:
				function = "require";
				is_function = 1;
				break;
			case ZEND_REQUIRE_ONCE:
				function = "require_once";
				is_function = 1;
				break;
			default:
				function = "Unknown";
		}
	} else {
		function = get_active_function_name();
		if (!function || !strlen(function)) {
			function = "Unknown";
		} else {
			is_function = 1;
			class_name = get_active_class_name(&space);
		}
	}

	/* if we still have memory then format the origin */
	if (is_function) {
		origin_len = (int)spprintf(&origin, 0, "%s%s%s(%s)", class_name, space, function, params);
	} else {
		origin_len = (int)spprintf(&origin, 0, "%s", function);
	}

	if (PG(html_errors)) {
		replace_origin = php_escape_html_entities((unsigned char*)origin, origin_len, 0, ENT_COMPAT, NULL);
		efree(origin);
		origin = ZSTR_VAL(replace_origin);
	}

	/* origin and buffer available, so lets come up with the error message */
	if (docref && docref[0] == '#') {
		docref_target = strchr(docref, '#');
		docref = NULL;
	}

	/* no docref given but function is known (the default) */
	if (!docref && is_function) {
		int doclen;
		while (*function == '_') {
			function++;
		}
		if (space[0] == '\0') {
			doclen = (int)spprintf(&docref_buf, 0, "function.%s", function);
		} else {
			doclen = (int)spprintf(&docref_buf, 0, "%s.%s", class_name, function);
		}
		while((p = strchr(docref_buf, '_')) != NULL) {
			*p = '-';
		}
		docref = php_strtolower(docref_buf, doclen);
	}

	/* we have a docref for a function AND
	 * - we show errors in html mode AND
	 * - the user wants to see the links
	 */
	if (docref && is_function && PG(html_errors) && strlen(PG(docref_root))) {
		if (strncmp(docref, "http://", 7)) {
			/* We don't have 'http://' so we use docref_root */

			char *ref;  /* temp copy for duplicated docref */

			docref_root = PG(docref_root);

			ref = estrdup(docref);
			if (docref_buf) {
				efree(docref_buf);
			}
			docref_buf = ref;
			/* strip of the target if any */
			p = strrchr(ref, '#');
			if (p) {
				target = estrdup(p);
				if (target) {
					docref_target = target;
					*p = '\0';
				}
			}
			/* add the extension if it is set in ini */
			if (PG(docref_ext) && strlen(PG(docref_ext))) {
				spprintf(&docref_buf, 0, "%s%s", ref, PG(docref_ext));
				efree(ref);
			}
			docref = docref_buf;
		}
		/* display html formatted or only show the additional links */
		if (PG(html_errors)) {
			spprintf(&message, 0, "%s [<a href='%s%s%s'>%s</a>]: %s", origin, docref_root, docref, docref_target, docref, buffer);
		} else {
			spprintf(&message, 0, "%s [%s%s%s]: %s", origin, docref_root, docref, docref_target, buffer);
		}
		if (target) {
			efree(target);
		}
	} else {
		spprintf(&message, 0, "%s: %s", origin, buffer);
	}
	if (replace_origin) {
		zend_string_free(replace_origin);
	} else {
		efree(origin);
	}
	if (docref_buf) {
		efree(docref_buf);
	}

	if (PG(track_errors) && module_initialized && EG(valid_symbol_table) &&
			(Z_TYPE(EG(user_error_handler)) == IS_UNDEF || !(EG(user_error_handler_error_reporting) & type))) {
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
	if (replace_buffer) {
		zend_string_free(replace_buffer);
	} else {
		if (buffer_len > 0) {
			efree(buffer);
		}
	}

	php_error(type, "%s", message);
	efree(message);
}