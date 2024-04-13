composite_glyphs_via_mask (void				*_dst,
			   cairo_operator_t		 op,
			   cairo_surface_t		*_src,
			   int				 src_x,
			   int				 src_y,
			   int				 dst_x,
			   int				 dst_y,
			   cairo_composite_glyphs_info_t *info)
{
    cairo_scaled_glyph_t *glyph_cache[64];
    pixman_image_t *white = _pixman_image_for_color (CAIRO_COLOR_WHITE);
    cairo_scaled_glyph_t *scaled_glyph;
    uint8_t buf[2048];
    pixman_image_t *mask;
    pixman_format_code_t format;
    cairo_status_t status;
    int i;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (unlikely (white == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    /* XXX convert the glyphs to common formats a8/a8r8g8b8 to hit
     * optimised paths through pixman. Should we increase the bit
     * depth of the target surface, we should reconsider the appropriate
     * mask formats.
     */

    status = _cairo_scaled_glyph_lookup (info->font,
					 info->glyphs[0].index,
					 CAIRO_SCALED_GLYPH_INFO_SURFACE,
					 &scaled_glyph);
    if (unlikely (status)) {
	pixman_image_unref (white);
	return status;
    }

    memset (glyph_cache, 0, sizeof (glyph_cache));
    glyph_cache[info->glyphs[0].index % ARRAY_LENGTH (glyph_cache)] = scaled_glyph;

    format = PIXMAN_a8;
    i = (info->extents.width + 3) & ~3;
    if (scaled_glyph->surface->base.content & CAIRO_CONTENT_COLOR) {
	format = PIXMAN_a8r8g8b8;
	i = info->extents.width * 4;
    }

    if (i * info->extents.height > (int) sizeof (buf)) {
	mask = pixman_image_create_bits (format,
					info->extents.width,
					info->extents.height,
					NULL, 0);
    } else {
	memset (buf, 0, i * info->extents.height);
	mask = pixman_image_create_bits (format,
					info->extents.width,
					info->extents.height,
					(uint32_t *)buf, i);
    }
    if (unlikely (mask == NULL)) {
	pixman_image_unref (white);
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);
    }

    status = CAIRO_STATUS_SUCCESS;
    for (i = 0; i < info->num_glyphs; i++) {
	unsigned long glyph_index = info->glyphs[i].index;
	int cache_index = glyph_index % ARRAY_LENGTH (glyph_cache);
	cairo_image_surface_t *glyph_surface;
	int x, y;

	scaled_glyph = glyph_cache[cache_index];
	if (scaled_glyph == NULL ||
	    _cairo_scaled_glyph_index (scaled_glyph) != glyph_index)
	{
	    status = _cairo_scaled_glyph_lookup (info->font, glyph_index,
						 CAIRO_SCALED_GLYPH_INFO_SURFACE,
						 &scaled_glyph);

	    if (unlikely (status)) {
		pixman_image_unref (mask);
		pixman_image_unref (white);
		return status;
	    }

	    glyph_cache[cache_index] = scaled_glyph;
	}

	glyph_surface = scaled_glyph->surface;
	if (glyph_surface->width && glyph_surface->height) {
	    if (glyph_surface->base.content & CAIRO_CONTENT_COLOR &&
		format == PIXMAN_a8) {
		pixman_image_t *ca_mask;

		format = PIXMAN_a8r8g8b8;
		ca_mask = pixman_image_create_bits (format,
						    info->extents.width,
						    info->extents.height,
						    NULL, 0);
		if (unlikely (ca_mask == NULL)) {
		    pixman_image_unref (mask);
		    pixman_image_unref (white);
		    return _cairo_error (CAIRO_STATUS_NO_MEMORY);
		}

		pixman_image_composite32 (PIXMAN_OP_SRC,
					  white, mask, ca_mask,
					  0, 0,
					  0, 0,
					  0, 0,
					  info->extents.width,
					  info->extents.height);
		pixman_image_unref (mask);
		mask = ca_mask;
	    }

	    /* round glyph locations to the nearest pixel */
	    /* XXX: FRAGILE: We're ignoring device_transform scaling here. A bug? */
	    x = _cairo_lround (info->glyphs[i].x -
			       glyph_surface->base.device_transform.x0);
	    y = _cairo_lround (info->glyphs[i].y -
			       glyph_surface->base.device_transform.y0);

	    if (glyph_surface->pixman_format == format) {
		pixman_image_composite32 (PIXMAN_OP_ADD,
					  glyph_surface->pixman_image, NULL, mask,
					  0, 0,
					  0, 0,
					  x - info->extents.x, y - info->extents.y,
					  glyph_surface->width,
					  glyph_surface->height);
	    } else {
		pixman_image_composite32 (PIXMAN_OP_ADD,
					  white, glyph_surface->pixman_image, mask,
					  0, 0,
					  0, 0,
					  x - info->extents.x, y - info->extents.y,
					  glyph_surface->width,
					  glyph_surface->height);
	    }
	}
    }

    if (format == PIXMAN_a8r8g8b8)
	pixman_image_set_component_alpha (mask, TRUE);

    pixman_image_composite32 (_pixman_operator (op),
			      ((cairo_image_source_t *)_src)->pixman_image,
			      mask,
			      to_pixman_image (_dst),
			      info->extents.x + src_x, info->extents.y + src_y,
			      0, 0,
			      info->extents.x - dst_x, info->extents.y - dst_y,
			      info->extents.width, info->extents.height);
    pixman_image_unref (mask);
    pixman_image_unref (white);

    return CAIRO_STATUS_SUCCESS;
}