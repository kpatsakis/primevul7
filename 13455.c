_cairo_image_scaled_glyph_fini (cairo_scaled_font_t *scaled_font,
				cairo_scaled_glyph_t *scaled_glyph)
{
    CAIRO_MUTEX_LOCK (_cairo_glyph_cache_mutex);

    if (global_glyph_cache) {
	pixman_glyph_cache_remove (
	    global_glyph_cache, scaled_font,
	    (void *)scaled_glyph->hash_entry.hash);
    }

    CAIRO_MUTEX_UNLOCK (_cairo_glyph_cache_mutex);
}