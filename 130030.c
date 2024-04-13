token_bounds_check (MonoImage *image, guint32 token)
{
	if (image->dynamic)
		return mono_reflection_is_valid_dynamic_token ((MonoDynamicImage*)image, token);
	return image->tables [mono_metadata_token_table (token)].rows >= mono_metadata_token_index (token);
}