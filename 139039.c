text_convert_invalid (const gchar* text, gssize len, GIConv converter, const gchar *fallback, gsize *len_out)
{
	gchar *result_part;
	gsize result_part_len;
	const gchar *end;
	gsize invalid_start_pos;
	GString *result;
	const gchar *current_start;

	if (len == -1)
	{
		len = strlen (text);
	}

	end = text + len;

	/* Find the first position of an invalid sequence. */
	result_part = g_convert_with_iconv (text, len, converter, &invalid_start_pos, &result_part_len, NULL);
	g_iconv (converter, NULL, NULL, NULL, NULL);

	if (result_part != NULL)
	{
		/* All text converted successfully on the first try. Return it. */

		if (len_out != NULL)
		{
			*len_out = result_part_len;
		}

		return result_part;
	}

	/* One or more invalid sequences exist that need to be replaced with the fallback. */

	result = g_string_sized_new (len);
	current_start = text;

	for (;;)
	{
		g_assert (current_start + invalid_start_pos < end);

		/* Convert everything before the position of the invalid sequence. It should be successful.
		 * But iconv may not convert everything till invalid_start_pos since the last few bytes may be part of a shift sequence.
		 * So get the new bytes_read and use it as the actual invalid_start_pos to handle this.
		 *
		 * See https://github.com/hexchat/hexchat/issues/1758
		 */
		result_part = g_convert_with_iconv (current_start, invalid_start_pos, converter, &invalid_start_pos, &result_part_len, NULL);
		g_iconv (converter, NULL, NULL, NULL, NULL);

		g_assert (result_part != NULL);
		g_string_append_len (result, result_part, result_part_len);
		g_free (result_part);

		/* Append the fallback */
		g_string_append (result, fallback);

		/* Now try converting everything after the invalid sequence. */
		current_start += invalid_start_pos + 1;

		result_part = g_convert_with_iconv (current_start, end - current_start, converter, &invalid_start_pos, &result_part_len, NULL);
		g_iconv (converter, NULL, NULL, NULL, NULL);

		if (result_part != NULL)
		{
			/* The rest of the text converted successfully. Append it and return the whole converted text. */

			g_string_append_len (result, result_part, result_part_len);
			g_free (result_part);

			if (len_out != NULL)
			{
				*len_out = result->len;
			}

			return g_string_free (result, FALSE);
		}

		/* The rest of the text didn't convert successfully. invalid_start_pos has the position of the next invalid sequence. */
	}
}