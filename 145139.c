void php_module_shutdown(void)
{
	int module_number=0;	/* for UNREGISTER_INI_ENTRIES() */

	module_shutdown = 1;

	if (!module_initialized) {
		return;
	}

#ifdef ZTS
	ts_free_worker_threads();
#endif

#if defined(PHP_WIN32) || (defined(NETWARE) && defined(USE_WINSOCK))
	/*close winsock */
	WSACleanup();
#endif

#ifdef PHP_WIN32
	php_win32_free_rng_lock();
#endif

	sapi_flush();

	zend_shutdown();

	/* Destroys filter & transport registries too */
	php_shutdown_stream_wrappers(module_number);

	UNREGISTER_INI_ENTRIES();

	/* close down the ini config */
	php_shutdown_config();

#ifndef ZTS
	zend_ini_shutdown();
	shutdown_memory_manager(CG(unclean_shutdown), 1);
#else
	zend_ini_global_shutdown();
#endif

	php_output_shutdown();

	module_initialized = 0;

#ifndef ZTS
	core_globals_dtor(&core_globals);
	gc_globals_dtor();
#else
	ts_free_id(core_globals_id);
#endif

#ifdef PHP_WIN32
	if (old_invalid_parameter_handler == NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}
#endif
}