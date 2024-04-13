SAPI_API SAPI_POST_HANDLER_FUNC(php_std_post_handler)
{
	zval *arr = (zval *) arg;
	php_stream *s = SG(request_info).request_body;
	post_var_data_t post_data;

	if (s && SUCCESS == php_stream_rewind(s)) {
		memset(&post_data, 0, sizeof(post_data));

		while (!php_stream_eof(s)) {
			char buf[BUFSIZ] = {0};
			size_t len = php_stream_read(s, buf, BUFSIZ);

			if (len && len != (size_t) -1) {
				smart_str_appendl(&post_data.str, buf, len);

				if (SUCCESS != add_post_vars(arr, &post_data, 0 TSRMLS_CC)) {
					if (post_data.str.c) {
						efree(post_data.str.c);
					}
					return;
				}
			}

			if (len != BUFSIZ){
				break;
			}
		}

		add_post_vars(arr, &post_data, 1 TSRMLS_CC);
		if (post_data.str.c) {
			efree(post_data.str.c);
		}
	}
}