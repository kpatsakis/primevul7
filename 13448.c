static void free_pixels (pixman_image_t *image, void *data)
{
	free (data);
}