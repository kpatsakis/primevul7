static php_stream* http_connect(zval* this_ptr, php_url *phpurl, int use_ssl, php_stream_context *context, int *use_proxy TSRMLS_DC)
{
	php_stream *stream;
	zval **proxy_host, **proxy_port, **tmp;
	char *host;
	char *name;
	long namelen;
	int port;
	int old_error_reporting;
	struct timeval tv;
	struct timeval *timeout = NULL;

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_host", sizeof("_proxy_host"), (void **) &proxy_host) == SUCCESS &&
	    Z_TYPE_PP(proxy_host) == IS_STRING &&
	    zend_hash_find(Z_OBJPROP_P(this_ptr), "_proxy_port", sizeof("_proxy_port"), (void **) &proxy_port) == SUCCESS &&
	    Z_TYPE_PP(proxy_port) == IS_LONG) {
		host = Z_STRVAL_PP(proxy_host);
		port = Z_LVAL_PP(proxy_port);
		*use_proxy = 1;
	} else {
		host = phpurl->host;
		port = phpurl->port;
	}
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_connection_timeout", sizeof("_connection_timeout"), (void **) &tmp) == SUCCESS &&
	    Z_TYPE_PP(tmp) == IS_LONG && Z_LVAL_PP(tmp) > 0) {
	  tv.tv_sec = Z_LVAL_PP(tmp);
	  tv.tv_usec = 0;
		timeout = &tv;
	}

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	namelen = spprintf(&name, 0, "%s://%s:%d", (use_ssl && !*use_proxy)? "ssl" : "tcp", host, port);

	stream = php_stream_xport_create(name, namelen,
		REPORT_ERRORS,
		STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
		NULL /*persistent_id*/,
		timeout,
		context,
		NULL, NULL);
	efree(name);

	/* SSL & proxy */
	if (stream && *use_proxy && use_ssl) {
		smart_str soap_headers = {0};
		char *http_headers;
		int http_header_size;

		smart_str_append_const(&soap_headers, "CONNECT ");
		smart_str_appends(&soap_headers, phpurl->host);
		smart_str_appendc(&soap_headers, ':');
		smart_str_append_unsigned(&soap_headers, phpurl->port);
		smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != 80) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		smart_str_append_const(&soap_headers, "\r\n");
		proxy_authentication(this_ptr, &soap_headers TSRMLS_CC);
		smart_str_append_const(&soap_headers, "\r\n");
		if (php_stream_write(stream, soap_headers.c, soap_headers.len) != soap_headers.len) {
			php_stream_close(stream);
			stream = NULL;
		}
 	 	smart_str_free(&soap_headers);

 	 	if (stream) {
			if (!get_http_headers(stream, &http_headers, &http_header_size TSRMLS_CC) || http_headers == NULL) {
				php_stream_close(stream);
				stream = NULL;
			}
			if (http_headers) {
				efree(http_headers);
			}
		}
		/* enable SSL transport layer */
		if (stream) {
			if (php_stream_xport_crypto_setup(stream, STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0 ||
			    php_stream_xport_crypto_enable(stream, 1 TSRMLS_CC) < 0) {
				php_stream_close(stream);
				stream = NULL;
			}
		}
	}

	EG(error_reporting) = old_error_reporting;
	return stream;
}