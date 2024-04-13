_g_str_escape (const char *str,
	       const char *meta_chars)
{
	return _g_str_escape_full (str, meta_chars, '\\', 0);
}