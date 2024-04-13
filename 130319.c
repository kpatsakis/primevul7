static long sfile_seek(jas_stream_obj_t *obj, long offset, int origin)
{
	FILE *fp;
	fp = JAS_CAST(FILE *, obj);
	return fseek(fp, offset, origin);
}