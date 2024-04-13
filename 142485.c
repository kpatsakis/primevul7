SAPI_API void sapi_shutdown(void)
{
#ifdef ZTS
	ts_free_id(sapi_globals_id);
#else
	sapi_globals_dtor(&sapi_globals);
#endif

	reentrancy_shutdown();

	virtual_cwd_shutdown();

#ifdef PHP_WIN32
	tsrm_win32_shutdown();
#endif
}