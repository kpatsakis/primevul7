composite_traps (void			*_dst,
		 cairo_operator_t	op,
		 cairo_surface_t	*abstract_src,
		 int			src_x,
		 int			src_y,
		 int			dst_x,
		 int			dst_y,
		 const cairo_rectangle_int_t *extents,
		 cairo_antialias_t	antialias,
		 cairo_traps_t		*traps)
{
    cairo_image_surface_t *dst = (cairo_image_surface_t *) _dst;
    cairo_image_source_t *src = (cairo_image_source_t *) abstract_src;
    cairo_int_status_t status;
    pixman_image_t *mask;
    pixman_format_code_t format;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    /* pixman doesn't eliminate self-intersecting trapezoids/edges */
    status = _cairo_bentley_ottmann_tessellate_traps (traps,
						      CAIRO_FILL_RULE_WINDING);
    if (status != CAIRO_INT_STATUS_SUCCESS)
	    return status;

    /* Special case adding trapezoids onto a mask surface; we want to avoid
     * creating an intermediate temporary mask unnecessarily.
     *
     * We make the assumption here that the portion of the trapezoids
     * contained within the surface is bounded by [dst_x,dst_y,width,height];
     * the Cairo core code passes bounds based on the trapezoid extents.
     */
    format = antialias == CAIRO_ANTIALIAS_NONE ? PIXMAN_a1 : PIXMAN_a8;
    if (dst->pixman_format == format &&
	(abstract_src == NULL ||
	 (op == CAIRO_OPERATOR_ADD && src->is_opaque_solid)))
    {
	_pixman_image_add_traps (dst->pixman_image, dst_x, dst_y, traps);
	return CAIRO_STATUS_SUCCESS;
    }

    mask = pixman_image_create_bits (format,
				     extents->width, extents->height,
				     NULL, 0);
    if (unlikely (mask == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    _pixman_image_add_traps (mask, extents->x, extents->y, traps);
    pixman_image_composite32 (_pixman_operator (op),
                              src->pixman_image, mask, dst->pixman_image,
                              extents->x + src_x, extents->y + src_y,
                              0, 0,
                              extents->x - dst_x, extents->y - dst_y,
                              extents->width, extents->height);

    pixman_image_unref (mask);

    return  CAIRO_STATUS_SUCCESS;
}