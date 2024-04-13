static int sfile_read(jas_stream_obj_t *obj, char *buf, int cnt)
{
	FILE *fp;
	fp = JAS_CAST(FILE *, obj);
	return fread(buf, 1, cnt, fp);
}