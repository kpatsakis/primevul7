static long file_seek(jas_stream_obj_t *obj, long offset, int origin)
{
	jas_stream_fileobj_t *fileobj = JAS_CAST(jas_stream_fileobj_t *, obj);
	return lseek(fileobj->fd, offset, origin);
}