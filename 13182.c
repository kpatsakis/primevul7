fsobj_error(int *a_eno, struct archive_string *a_estr,
    int err, const char *errstr, const char *path)
{
	if (a_eno)
		*a_eno = err;
	if (a_estr)
		archive_string_sprintf(a_estr, "%s%s", errstr, path);
}