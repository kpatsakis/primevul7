add_mono_string_to_blob_cached (MonoDynamicImage *assembly, MonoString *str)
{
	char blob_size [64];
	char *b = blob_size;
	guint32 idx = 0, len;

	len = str->length * 2;
	mono_metadata_encode_value (len, b, &b);
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	{
		char *swapped = g_malloc (2 * mono_string_length (str));
		const char *p = (const char*)mono_string_chars (str);

		swap_with_size (swapped, p, 2, mono_string_length (str));
		idx = add_to_blob_cached (assembly, blob_size, b-blob_size, swapped, len);
		g_free (swapped);
	}
#else
	idx = add_to_blob_cached (assembly, blob_size, b-blob_size, (char*)mono_string_chars (str), len);
#endif
	return idx;
}