get_glyph_cache (void)
{
    if (!global_glyph_cache)
	global_glyph_cache = pixman_glyph_cache_create ();

    return global_glyph_cache;
}