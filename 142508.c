SAPI_API double sapi_get_request_time(TSRMLS_D)
{
	if(SG(global_request_time)) return SG(global_request_time);

	if (sapi_module.get_request_time && SG(server_context)) {
		SG(global_request_time) = sapi_module.get_request_time(TSRMLS_C);
	} else {
		struct timeval tp = {0};
		if (!gettimeofday(&tp, NULL)) {
			SG(global_request_time) = (double)(tp.tv_sec + tp.tv_usec / 1000000.00);
		} else {
			SG(global_request_time) = (double)time(0);
		}
	}
	return SG(global_request_time);
}