cvt_id3(struct magic_set *ms, uint32_t v)
{
	v = ((((v >>  0) & 0x7f) <<  0) |
	     (((v >>  8) & 0x7f) <<  7) |
	     (((v >> 16) & 0x7f) << 14) |
	     (((v >> 24) & 0x7f) << 21));
	if ((ms->flags & MAGIC_DEBUG) != 0)
		fprintf(stderr, "id3 offs=%u\n", v);
	return v;
}