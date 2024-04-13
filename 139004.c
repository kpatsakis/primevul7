text_fixup_invalid_utf8 (const gchar* text, gssize len, gsize *len_out)
{
	static GIConv utf8_fixup_converter = NULL;
	if (utf8_fixup_converter == NULL)
	{
		utf8_fixup_converter = g_iconv_open ("UTF-8", "UTF-8");
	}

	return text_convert_invalid (text, len, utf8_fixup_converter, unicode_fallback_string, len_out);
}