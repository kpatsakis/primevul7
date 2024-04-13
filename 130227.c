image_strdup (MonoImage *image, const char *s)
{
	if (image)
		return mono_image_strdup (image, s);
	else
		return g_strdup (s);
}