bufadd(struct buffer *buf, char c)
{
	if (buf->len >= buf->cap) {
		buf->cap = buf->cap ? buf->cap * 2 : 1 << 8;
		buf->data = realloc(buf->data, buf->cap);
		if (!buf->data)
			fatal("realloc:");
	}
	buf->data[buf->len++] = c;
}