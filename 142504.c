SAPI_API void sapi_terminate_process(TSRMLS_D) {
	if (sapi_module.terminate_process) {
		sapi_module.terminate_process(TSRMLS_C);
	}
}