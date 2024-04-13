void php_request_shutdown(void *dummy)
{
	zend_bool report_memleaks;

	report_memleaks = PG(report_memleaks);

	/* EG(current_execute_data) points into nirvana and therefore cannot be safely accessed
	 * inside zend_executor callback functions.
	 */
	EG(current_execute_data) = NULL;

	php_deactivate_ticks();

	/* 1. Call all possible shutdown functions registered with register_shutdown_function() */
	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions();
	} zend_end_try();

	/* 2. Call all possible __destruct() functions */
	zend_try {
		zend_call_destructors();
	} zend_end_try();

	/* 3. Flush all output buffers */
	zend_try {
		zend_bool send_buffer = SG(request_info).headers_only ? 0 : 1;

		if (CG(unclean_shutdown) && PG(last_error_type) == E_ERROR &&
			(size_t)PG(memory_limit) < zend_memory_usage(1)
		) {
			send_buffer = 0;
		}

		if (!send_buffer) {
			php_output_discard_all();
		} else {
			php_output_end_all();
		}
	} zend_end_try();

	/* 4. Reset max_execution_time (no longer executing php code after response sent) */
	zend_try {
		zend_unset_timeout();
	} zend_end_try();

	/* 5. Call all extensions RSHUTDOWN functions */
	if (PG(modules_activated)) {
		zend_deactivate_modules();
		php_free_shutdown_functions();
	}

	/* 6. Shutdown output layer (send the set HTTP headers, cleanup output handlers, etc.) */
	zend_try {
		php_output_deactivate();
	} zend_end_try();

	/* 7. Destroy super-globals */
	zend_try {
		int i;

		for (i=0; i<NUM_TRACK_VARS; i++) {
			zval_ptr_dtor(&PG(http_globals)[i]);
		}
	} zend_end_try();

	/* 8. free request-bound globals */
	php_free_request_globals();

	/* 9. Shutdown scanner/executor/compiler and restore ini entries */
	zend_deactivate();

	/* 10. Call all extensions post-RSHUTDOWN functions */
	zend_try {
		zend_post_deactivate_modules();
	} zend_end_try();

	/* 11. SAPI related shutdown (free stuff) */
	zend_try {
		sapi_deactivate();
	} zend_end_try();

	/* 12. free virtual CWD memory */
	virtual_cwd_deactivate();

	/* 13. Destroy stream hashes */
	zend_try {
		php_shutdown_stream_hashes();
	} zend_end_try();

	/* 14. Free Willy (here be crashes) */
	zend_interned_strings_restore();
	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown) || !report_memleaks, 0);
	} zend_end_try();

	/* 15. Reset max_execution_time */
	zend_try {
		zend_unset_timeout();
	} zend_end_try();

#ifdef PHP_WIN32
	if (PG(com_initialized)) {
		CoUninitialize();
		PG(com_initialized) = 0;
	}
#endif

#ifdef HAVE_DTRACE
	DTRACE_REQUEST_SHUTDOWN(SAFE_FILENAME(SG(request_info).path_translated), SAFE_FILENAME(SG(request_info).request_uri), (char *)SAFE_FILENAME(SG(request_info).request_method));
#endif /* HAVE_DTRACE */
}