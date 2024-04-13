
static void php_date(INTERNAL_FUNCTION_PARAMETERS, int localtime)
{
	char   *format;
	int     format_len;
	long    ts;
	char   *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}
	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}

	string = php_format_date(format, format_len, ts, localtime TSRMLS_CC);
	
	RETVAL_STRING(string, 0);