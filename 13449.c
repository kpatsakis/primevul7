set_clip_region (void *_surface,
		 cairo_region_t *region)
{
    cairo_image_surface_t *surface = _surface;
    pixman_region32_t *rgn = region ? &region->rgn : NULL;

    if (! pixman_image_set_clip_region32 (surface->pixman_image, rgn))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    return CAIRO_STATUS_SUCCESS;
}