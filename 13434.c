composite_tristrip (void			*_dst,
		    cairo_operator_t	op,
		    cairo_surface_t	*abstract_src,
		    int			src_x,
		    int			src_y,
		    int			dst_x,
		    int			dst_y,
		    const cairo_rectangle_int_t *extents,
		    cairo_antialias_t	antialias,
		    cairo_tristrip_t	*strip)
{
    cairo_image_surface_t *dst = (cairo_image_surface_t *) _dst;
    cairo_image_source_t *src = (cairo_image_source_t *) abstract_src;
    pixman_image_t *mask;
    pixman_format_code_t format;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (strip->num_points < 3)
	return CAIRO_STATUS_SUCCESS;

    if (1) { /* pixman doesn't eliminate self-intersecting triangles/edges */
	    cairo_int_status_t status;
	    cairo_traps_t traps;
	    int n;

	    _cairo_traps_init (&traps);
	    for (n = 0; n < strip->num_points; n++) {
		    cairo_point_t p[4];

		    p[0] = strip->points[0];
		    p[1] = strip->points[1];
		    p[2] = strip->points[2];
		    p[3] = strip->points[0];

		    _cairo_traps_tessellate_convex_quad (&traps, p);
	    }
	    status = composite_traps (_dst, op, abstract_src,
				      src_x, src_y,
				      dst_x, dst_y,
				      extents, antialias, &traps);
	    _cairo_traps_fini (&traps);

	    return status;
    }

    format = antialias == CAIRO_ANTIALIAS_NONE ? PIXMAN_a1 : PIXMAN_a8;
    if (dst->pixman_format == format &&
	(abstract_src == NULL ||
	 (op == CAIRO_OPERATOR_ADD && src->is_opaque_solid)))
    {
	_pixman_image_add_tristrip (dst->pixman_image, dst_x, dst_y, strip);
	return CAIRO_STATUS_SUCCESS;
    }

    mask = pixman_image_create_bits (format,
				     extents->width, extents->height,
				     NULL, 0);
    if (unlikely (mask == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    _pixman_image_add_tristrip (mask, extents->x, extents->y, strip);
    pixman_image_composite32 (_pixman_operator (op),
                              src->pixman_image, mask, dst->pixman_image,
                              extents->x + src_x, extents->y + src_y,
                              0, 0,
                              extents->x - dst_x, extents->y - dst_y,
                              extents->width, extents->height);

    pixman_image_unref (mask);

    return  CAIRO_STATUS_SUCCESS;
}