void diff_free_filespec_data(struct diff_filespec *s)
{
	diff_free_filespec_blob(s);
	free(s->cnt_data);
	s->cnt_data = NULL;
}