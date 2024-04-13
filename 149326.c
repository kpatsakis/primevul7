file_fmtcheck(struct magic_set *ms, const char *desc, const char *def,
	const char *file, size_t line)
{
	const char *ptr = fmtcheck(desc, def);
	if (ptr == def)
		file_magerror(ms,
		    "%s, %" SIZE_T_FORMAT "u: format `%s' does not match"
		    " with `%s'", file, line, desc, def);
	return ptr;
}