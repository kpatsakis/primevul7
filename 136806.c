static void phar_do_403(char *entry, int entry_len) /* {{{ */
{
	sapi_header_line ctr = {0};

	ctr.response_code = 403;
	ctr.line_len = sizeof("HTTP/1.0 403 Access Denied")-1;
	ctr.line = "HTTP/1.0 403 Access Denied";
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
	sapi_send_headers();
	PHPWRITE("<html>\n <head>\n  <title>Access Denied</title>\n </head>\n <body>\n  <h1>403 - File ", sizeof("<html>\n <head>\n  <title>Access Denied</title>\n </head>\n <body>\n  <h1>403 - File ") - 1);
	PHPWRITE("Access Denied</h1>\n </body>\n</html>", sizeof("Access Denied</h1>\n </body>\n</html>") - 1);
}