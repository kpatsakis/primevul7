file_badread(struct magic_set *ms)
{
	file_error(ms, errno, "error reading");
}