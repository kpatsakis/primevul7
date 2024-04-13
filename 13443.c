_cairo_image_compositor_reset_static_data (void)
{
    CAIRO_MUTEX_LOCK (_cairo_glyph_cache_mutex);

    if (global_glyph_cache)
	pixman_glyph_cache_destroy (global_glyph_cache);
    global_glyph_cache = NULL;

    CAIRO_MUTEX_UNLOCK (_cairo_glyph_cache_mutex);
}