void php_filter_validate_domain(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	if (!_php_filter_validate_domain(Z_STRVAL_P(value), Z_STRLEN_P(value), flags)) {
		RETURN_VALIDATION_FAILED
	}
}