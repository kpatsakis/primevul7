sigbuffer_add_mem (SigBuffer *buf, char *p, guint32 size)
{
	sigbuffer_make_room (buf, size);
	memcpy (buf->p, p, size);
	buf->p += size;
}