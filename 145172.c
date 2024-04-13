PHPAPI void php_com_initialize(void)
{
#ifdef PHP_WIN32
	if (!PG(com_initialized)) {
		if (CoInitialize(NULL) == S_OK) {
			PG(com_initialized) = 1;
		}
	}
#endif
}