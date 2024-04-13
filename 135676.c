static void diff_words_append(char *line, unsigned long len,
		struct diff_words_buffer *buffer)
{
	if (buffer->text.size + len > buffer->alloc) {
		buffer->alloc = (buffer->text.size + len) * 3 / 2;
		buffer->text.ptr = xrealloc(buffer->text.ptr, buffer->alloc);
	}
	line++;
	len--;
	memcpy(buffer->text.ptr + buffer->text.size, line, len);
	buffer->text.size += len;
}