static void fn_out_diff_words_aux(void *priv, char *line, unsigned long len)
{
	struct diff_words_data *diff_words = priv;

	if (diff_words->minus.suppressed_newline) {
		if (line[0] != '+')
			putc('\n', diff_words->file);
		diff_words->minus.suppressed_newline = 0;
	}

	len--;
	switch (line[0]) {
		case '-':
			print_word(diff_words->file,
				   &diff_words->minus, len, DIFF_FILE_OLD, 1);
			break;
		case '+':
			print_word(diff_words->file,
				   &diff_words->plus, len, DIFF_FILE_NEW, 0);
			break;
		case ' ':
			print_word(diff_words->file,
				   &diff_words->plus, len, DIFF_PLAIN, 0);
			diff_words->minus.current += len;
			break;
	}
}