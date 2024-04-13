static void free_diff_words_data(struct emit_callback *ecbdata)
{
	if (ecbdata->diff_words) {
		/* flush buffers */
		if (ecbdata->diff_words->minus.text.size ||
				ecbdata->diff_words->plus.text.size)
			diff_words_show(ecbdata->diff_words);

		free (ecbdata->diff_words->minus.text.ptr);
		free (ecbdata->diff_words->plus.text.ptr);
		free(ecbdata->diff_words);
		ecbdata->diff_words = NULL;
	}
}