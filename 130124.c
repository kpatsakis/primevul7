image_g_malloc (MonoImage *image, guint size)
{
	if (image)
		return mono_image_alloc (image, size);
	else
		return g_malloc (size);
}