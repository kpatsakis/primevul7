static int sfile_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
	FILE *fp;
	fp = JAS_CAST(FILE *, obj);
	return fwrite(buf, 1, cnt, fp);
}