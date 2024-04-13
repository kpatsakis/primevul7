static void print_word(FILE *file, struct diff_words_buffer *buffer, int len, int color,
		int suppress_newline)
{
	const char *ptr;
	int eol = 0;

	if (len == 0)
		return;

	ptr  = buffer->text.ptr + buffer->current;
	buffer->current += len;

	if (ptr[len - 1] == '\n') {
		eol = 1;
		len--;
	}

	fputs(diff_get_color(1, color), file);
	fwrite(ptr, len, 1, file);
	fputs(diff_get_color(1, DIFF_RESET), file);

	if (eol) {
		if (suppress_newline)
			buffer->suppressed_newline = 1;
		else
			putc('\n', file);
	}
}