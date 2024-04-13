void php_filter_validate_regexp(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval *option_val;
	zend_string *regexp;
	int regexp_set;
	pcre2_code *re = NULL;
	pcre2_match_data *match_data = NULL;
	uint32_t preg_options, capture_count;
	int rc;

	/* Parse options */
	FETCH_STR_OPTION(regexp, "regexp");

	if (!regexp_set) {
		php_error_docref(NULL, E_WARNING, "'regexp' option missing");
		RETURN_VALIDATION_FAILED
	}

	re = pcre_get_compiled_regex(regexp, &capture_count, &preg_options);
	if (!re) {
		RETURN_VALIDATION_FAILED
	}
	match_data = php_pcre_create_match_data(capture_count, re);
	if (!match_data) {
		RETURN_VALIDATION_FAILED
	}
	rc = pcre2_match(re, (PCRE2_SPTR)Z_STRVAL_P(value), Z_STRLEN_P(value), 0, preg_options, match_data, php_pcre_mctx());
	php_pcre_free_match_data(match_data);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (rc < 0) {
		RETURN_VALIDATION_FAILED
	}
}