composite_glyphs (void				*_dst,
		  cairo_operator_t		 op,
		  cairo_surface_t		*_src,
		  int				 src_x,
		  int				 src_y,
		  int				 dst_x,
		  int				 dst_y,
		  cairo_composite_glyphs_info_t *info)
{
    cairo_scaled_glyph_t *glyph_cache[64];
    pixman_image_t *dst, *src;
    cairo_status_t status;
    int i;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (info->num_glyphs == 1)
	return composite_one_glyph(_dst, op, _src, src_x, src_y, dst_x, dst_y, info);

    if (info->use_mask)
	return composite_glyphs_via_mask(_dst, op, _src, src_x, src_y, dst_x, dst_y, info);

    op = _pixman_operator (op);
    dst = to_pixman_image (_dst);
    src = ((cairo_image_source_t *)_src)->pixman_image;

    memset (glyph_cache, 0, sizeof (glyph_cache));
    status = CAIRO_STATUS_SUCCESS;

    for (i = 0; i < info->num_glyphs; i++) {
	int x, y;
	cairo_image_surface_t *glyph_surface;
	cairo_scaled_glyph_t *scaled_glyph;
	unsigned long glyph_index = info->glyphs[i].index;
	int cache_index = glyph_index % ARRAY_LENGTH (glyph_cache);

	scaled_glyph = glyph_cache[cache_index];
	if (scaled_glyph == NULL ||
	    _cairo_scaled_glyph_index (scaled_glyph) != glyph_index)
	{
	    status = _cairo_scaled_glyph_lookup (info->font, glyph_index,
						 CAIRO_SCALED_GLYPH_INFO_SURFACE,
						 &scaled_glyph);

	    if (unlikely (status))
		break;

	    glyph_cache[cache_index] = scaled_glyph;
	}

	glyph_surface = scaled_glyph->surface;
	if (glyph_surface->width && glyph_surface->height) {
	    /* round glyph locations to the nearest pixel */
	    /* XXX: FRAGILE: We're ignoring device_transform scaling here. A bug? */
	    x = _cairo_lround (info->glyphs[i].x -
			       glyph_surface->base.device_transform.x0);
	    y = _cairo_lround (info->glyphs[i].y -
			       glyph_surface->base.device_transform.y0);

	    pixman_image_composite32 (op, src, glyph_surface->pixman_image, dst,
                                      x + src_x,  y + src_y,
                                      0, 0,
                                      x - dst_x, y - dst_y,
				      glyph_surface->width,
				      glyph_surface->height);
	}
    }

    return status;
}