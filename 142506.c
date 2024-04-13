SAPI_API void sapi_startup(sapi_module_struct *sf)
{
#ifdef ZEND_SIGNALS
	zend_signal_startup();
#endif

	sf->ini_entries = NULL;
	sapi_module = *sf;

#ifdef ZTS
	ts_allocate_id(&sapi_globals_id, sizeof(sapi_globals_struct), (ts_allocate_ctor) sapi_globals_ctor, (ts_allocate_dtor) sapi_globals_dtor);
# ifdef PHP_WIN32
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
# endif
#else
	sapi_globals_ctor(&sapi_globals);
#endif

	virtual_cwd_startup(); /* Could use shutdown to free the main cwd but it would just slow it down for CGI */

#ifdef PHP_WIN32
	tsrm_win32_startup();
#endif

	reentrancy_startup();
}