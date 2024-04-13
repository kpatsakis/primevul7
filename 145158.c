static void core_globals_dtor(php_core_globals *core_globals)
{
	if (core_globals->last_error_message) {
		free(core_globals->last_error_message);
	}
	if (core_globals->last_error_file) {
		free(core_globals->last_error_file);
	}
	if (core_globals->disable_functions) {
		free(core_globals->disable_functions);
	}
	if (core_globals->disable_classes) {
		free(core_globals->disable_classes);
	}
	if (core_globals->php_binary) {
		free(core_globals->php_binary);
	}

	php_shutdown_ticks();
}