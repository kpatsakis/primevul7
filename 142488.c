SAPI_API int sapi_send_headers(TSRMLS_D)
{
	int retval;
	int ret = FAILURE;

	if (SG(headers_sent) || SG(request_info).no_headers || SG(callback_run)) {
		return SUCCESS;
	}

	/* Success-oriented.  We set headers_sent to 1 here to avoid an infinite loop
	 * in case of an error situation.
	 */
	if (SG(sapi_headers).send_default_content_type && sapi_module.send_headers) {
		sapi_header_struct default_header;
	    uint len;

		SG(sapi_headers).mimetype = get_default_content_type(0, &len TSRMLS_CC);
		default_header.header_len = sizeof("Content-type: ") - 1 + len;
		default_header.header = emalloc(default_header.header_len + 1);
		memcpy(default_header.header, "Content-type: ", sizeof("Content-type: ") - 1);
		memcpy(default_header.header + sizeof("Content-type: ") - 1, SG(sapi_headers).mimetype, len + 1);
		sapi_header_add_op(SAPI_HEADER_ADD, &default_header TSRMLS_CC);
		SG(sapi_headers).send_default_content_type = 0;
	}

	if (SG(callback_func) && !SG(callback_run)) {
		SG(callback_run) = 1;
		sapi_run_header_callback(TSRMLS_C);
	}

	SG(headers_sent) = 1;

	if (sapi_module.send_headers) {
		retval = sapi_module.send_headers(&SG(sapi_headers) TSRMLS_CC);
	} else {
		retval = SAPI_HEADER_DO_SEND;
	}

	switch (retval) {
		case SAPI_HEADER_SENT_SUCCESSFULLY:
			ret = SUCCESS;
			break;
		case SAPI_HEADER_DO_SEND: {
				sapi_header_struct http_status_line;
				char buf[255];

				if (SG(sapi_headers).http_status_line) {
					http_status_line.header = SG(sapi_headers).http_status_line;
					http_status_line.header_len = strlen(SG(sapi_headers).http_status_line);
				} else {
					http_status_line.header = buf;
					http_status_line.header_len = slprintf(buf, sizeof(buf), "HTTP/1.0 %d X", SG(sapi_headers).http_response_code);
				}
				sapi_module.send_header(&http_status_line, SG(server_context) TSRMLS_CC);
			}
			zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t) sapi_module.send_header, SG(server_context) TSRMLS_CC);
			if(SG(sapi_headers).send_default_content_type) {
				sapi_header_struct default_header;

				sapi_get_default_content_type_header(&default_header TSRMLS_CC);
				sapi_module.send_header(&default_header, SG(server_context) TSRMLS_CC);
				sapi_free_header(&default_header);
			}
			sapi_module.send_header(NULL, SG(server_context) TSRMLS_CC);
			ret = SUCCESS;
			break;
		case SAPI_HEADER_SEND_FAILED:
			SG(headers_sent) = 0;
			ret = FAILURE;
			break;
	}

	sapi_send_headers_free(TSRMLS_C);

	return ret;
}