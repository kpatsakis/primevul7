static void sapi_header_add_op(sapi_header_op_enum op, sapi_header_struct *sapi_header TSRMLS_DC)
{
	if (!sapi_module.header_handler ||
		(SAPI_HEADER_ADD & sapi_module.header_handler(sapi_header, op, &SG(sapi_headers) TSRMLS_CC))) {
		if (op == SAPI_HEADER_REPLACE) {
			char *colon_offset = strchr(sapi_header->header, ':');

			if (colon_offset) {
				char sav = *colon_offset;

				*colon_offset = 0;
		        sapi_remove_header(&SG(sapi_headers).headers, sapi_header->header, strlen(sapi_header->header));
				*colon_offset = sav;
			}
		}
		zend_llist_add_element(&SG(sapi_headers).headers, (void *) sapi_header);
	} else {
		sapi_free_header(sapi_header);
	}
}