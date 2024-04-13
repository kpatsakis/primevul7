void dummy_invalid_parameter_handler(
		const wchar_t *expression,
		const wchar_t *function,
		const wchar_t *file,
		unsigned int   line,
		uintptr_t      pEwserved)
{
	static int called = 0;
	char buf[1024];
	int len;

	if (!called) {
			if(PG(windows_show_crt_warning)) {
			called = 1;
			if (function) {
				if (file) {
					len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws' (%ws:%u)", function, file, line);
				} else {
					len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws'", function);
				}
			} else {
				len = _snprintf(buf, sizeof(buf)-1, "Invalid CRT parameter detected (function not known)");
			}
			zend_error(E_WARNING, "%s", buf);
			called = 0;
		}
	}
}