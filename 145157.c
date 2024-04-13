static zend_string *php_resolve_path_for_zend(const char *filename, int filename_len) /* {{{ */
{
	return php_resolve_path(filename, filename_len, PG(include_path));
}