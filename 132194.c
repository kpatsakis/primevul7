static int get_http_body(php_stream *stream, int close, char *headers,  char **response, int *out_size TSRMLS_DC)
{
	char *header, *http_buf = NULL;
	int header_close = close, header_chunked = 0, header_length = 0, http_buf_size = 0;

	if (!close) {
		header = get_http_header_value(headers, "Connection: ");
		if (header) {
			if(!strncasecmp(header, "close", sizeof("close")-1)) header_close = 1;
			efree(header);
		}
	}
	header = get_http_header_value(headers, "Transfer-Encoding: ");
	if (header) {
		if(!strncasecmp(header, "chunked", sizeof("chunked")-1)) header_chunked = 1;
		efree(header);
	}
	header = get_http_header_value(headers, "Content-Length: ");
	if (header) {
		header_length = atoi(header);
		efree(header);
		if (!header_length && !header_chunked) {
			/* Empty response */
			http_buf = emalloc(1);
			http_buf[0] = '\0';
			(*response) = http_buf;
			(*out_size) = 0;
			return TRUE;
		}
	}

	if (header_chunked) {
		char ch, done, headerbuf[8192];

		done = FALSE;

		while (!done) {
			int buf_size = 0;

			php_stream_gets(stream, headerbuf, sizeof(headerbuf));
			if (sscanf(headerbuf, "%x", &buf_size) > 0 ) {
				if (buf_size > 0) {
					int len_size = 0;

					if (http_buf_size + buf_size + 1 < 0) {
						efree(http_buf);
						return FALSE;
					}
					http_buf = erealloc(http_buf, http_buf_size + buf_size + 1);

					while (len_size < buf_size) {
						int len_read = php_stream_read(stream, http_buf + http_buf_size, buf_size - len_size);
						if (len_read <= 0) {
							/* Error or EOF */
							done = TRUE;
						  break;
						}
						len_size += len_read;
	 					http_buf_size += len_read;
 					}

					/* Eat up '\r' '\n' */
					ch = php_stream_getc(stream);
					if (ch == '\r') {
						ch = php_stream_getc(stream);
					}
					if (ch != '\n') {
						/* Somthing wrong in chunked encoding */
						if (http_buf) {
							efree(http_buf);
						}
						return FALSE;
					}
 				}
			} else {
				/* Somthing wrong in chunked encoding */
				if (http_buf) {
					efree(http_buf);
				}
				return FALSE;
			}
			if (buf_size == 0) {
				done = TRUE;
			}
		}

		/* Ignore trailer headers */
		while (1) {
			if (!php_stream_gets(stream, headerbuf, sizeof(headerbuf))) {
				break;
			}

			if ((headerbuf[0] == '\r' && headerbuf[1] == '\n') ||
			    (headerbuf[0] == '\n')) {
				/* empty line marks end of headers */
				break;
			}
		}

		if (http_buf == NULL) {
			http_buf = emalloc(1);
		}

	} else if (header_length) {
		if (header_length < 0 || header_length >= INT_MAX) {
			return FALSE;
		}
		http_buf = safe_emalloc(1, header_length, 1);
		while (http_buf_size < header_length) {
			int len_read = php_stream_read(stream, http_buf + http_buf_size, header_length - http_buf_size);
			if (len_read <= 0) {
				break;
			}
			http_buf_size += len_read;
		}
	} else if (header_close) {
		do {
			int len_read;
			http_buf = erealloc(http_buf, http_buf_size + 4096 + 1);
			len_read = php_stream_read(stream, http_buf + http_buf_size, 4096);
			if (len_read > 0) {
				http_buf_size += len_read;
			}
		} while(!php_stream_eof(stream));
	} else {
		return FALSE;
	}

	http_buf[http_buf_size] = '\0';
	(*response) = http_buf;
	(*out_size) = http_buf_size;
	return TRUE;
}