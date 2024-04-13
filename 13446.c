composite_one_glyph (void				*_dst,
		     cairo_operator_t			 op,
		     cairo_surface_t			*_src,
		     int				 src_x,
		     int				 src_y,
		     int				 dst_x,
		     int				 dst_y,
		     cairo_composite_glyphs_info_t	 *info)
{
    cairo_image_surface_t *glyph_surface;
    cairo_scaled_glyph_t *scaled_glyph;
    cairo_status_t status;
    int x, y;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    status = _cairo_scaled_glyph_lookup (info->font,
					 info->glyphs[0].index,
					 CAIRO_SCALED_GLYPH_INFO_SURFACE,
					 &scaled_glyph);

    if (unlikely (status))
	return status;

    glyph_surface = scaled_glyph->surface;
    if (glyph_surface->width == 0 || glyph_surface->height == 0)
	return CAIRO_INT_STATUS_NOTHING_TO_DO;

    /* round glyph locations to the nearest pixel */
    /* XXX: FRAGILE: We're ignoring device_transform scaling here. A bug? */
    x = _cairo_lround (info->glyphs[0].x -
		       glyph_surface->base.device_transform.x0);
    y = _cairo_lround (info->glyphs[0].y -
		       glyph_surface->base.device_transform.y0);

    pixman_image_composite32 (_pixman_operator (op),
			      ((cairo_image_source_t *)_src)->pixman_image,
			      glyph_surface->pixman_image,
			      to_pixman_image (_dst),
			      x + src_x,  y + src_y,
			      0, 0,
			      x - dst_x, y - dst_y,
			      glyph_surface->width,
			      glyph_surface->height);

    return CAIRO_INT_STATUS_SUCCESS;
}