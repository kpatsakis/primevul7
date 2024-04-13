void php_request_shutdown_for_exec(void *dummy)
{

	/* used to close fd's in the 3..255 range here, but it's problematic
	 */
	shutdown_memory_manager(1, 1);
	zend_interned_strings_restore();
}