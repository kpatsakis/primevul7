static void sapi_update_response_code(int ncode TSRMLS_DC)
{
	/* if the status code did not change, we do not want
	   to change the status line, and no need to change the code */
	if (SG(sapi_headers).http_response_code == ncode) {
		return;
	}

	if (SG(sapi_headers).http_status_line) {
		efree(SG(sapi_headers).http_status_line);
		SG(sapi_headers).http_status_line = NULL;
	}
	SG(sapi_headers).http_response_code = ncode;
}