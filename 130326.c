static int sfile_close(jas_stream_obj_t *obj)
{
	FILE *fp;
	fp = JAS_CAST(FILE *, obj);
	return fclose(fp);
}