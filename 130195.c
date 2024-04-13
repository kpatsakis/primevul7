sigbuffer_add_value (SigBuffer *buf, guint32 val)
{
	sigbuffer_make_room (buf, 6);
	mono_metadata_encode_value (val, buf->p, &buf->p);
}