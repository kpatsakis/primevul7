ZEND_INI_MH(OnUpdateDecode)
{
	if (new_value) {
		const zend_encoding **return_list;
		size_t return_size;
		if (FAILURE == zend_multibyte_parse_encoding_list(new_value, new_value_length,
	&return_list, &return_size, 0 TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal encoding ignored: '%s'", new_value);
			return FAILURE;
		}
		efree(return_list);
	}
	return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}