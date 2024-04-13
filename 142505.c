SAPI_API void sapi_get_default_content_type_header(sapi_header_struct *default_header TSRMLS_DC)
{
    uint len;

	default_header->header = get_default_content_type(sizeof("Content-type: ")-1, &len TSRMLS_CC);
	default_header->header_len = len;
	memcpy(default_header->header, "Content-type: ", sizeof("Content-type: ") - 1);
}