void php_request_shutdown_for_hook(void *dummy)
{

	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions();
	} zend_end_try();

	if (PG(modules_activated)) {
		zend_deactivate_modules();
		php_free_shutdown_functions();
	}

	zend_try {
		zend_unset_timeout();
	} zend_end_try();

	zend_try {
		int i;

		for (i = 0; i < NUM_TRACK_VARS; i++) {
			zval_ptr_dtor(&PG(http_globals)[i]);
		}
	} zend_end_try();

	zend_deactivate();

	zend_try {
		sapi_deactivate();
	} zend_end_try();

	zend_try {
		php_shutdown_stream_hashes();
	} zend_end_try();

	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown), 0);
	} zend_end_try();

	zend_interned_strings_restore();

#ifdef ZEND_SIGNALS
	zend_try {
		zend_signal_deactivate();
	} zend_end_try();
#endif
}