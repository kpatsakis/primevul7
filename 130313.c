static int file_close(jas_stream_obj_t *obj)
{
	jas_stream_fileobj_t *fileobj = JAS_CAST(jas_stream_fileobj_t *, obj);
	int ret;
	ret = close(fileobj->fd);
	if (fileobj->flags & JAS_STREAM_FILEOBJ_DELONCLOSE) {
		unlink(fileobj->pathname);
	}
	jas_free(fileobj);
	return ret;
}