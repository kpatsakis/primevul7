_g_str_shell_escape (const char *filename)
{
	return _g_str_escape (filename, "$'`\"\\!?* ()[]&|:;<>#");
}