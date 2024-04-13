static int file_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
	jas_stream_fileobj_t *fileobj = JAS_CAST(jas_stream_fileobj_t *, obj);
	return write(fileobj->fd, buf, cnt);
}