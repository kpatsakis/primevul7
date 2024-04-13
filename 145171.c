static void php_free_request_globals(void)
{
	if (PG(last_error_message)) {
		free(PG(last_error_message));
		PG(last_error_message) = NULL;
	}
	if (PG(last_error_file)) {
		free(PG(last_error_file));
		PG(last_error_file) = NULL;
	}
	if (PG(php_sys_temp_dir)) {
		efree(PG(php_sys_temp_dir));
		PG(php_sys_temp_dir) = NULL;
	}
}