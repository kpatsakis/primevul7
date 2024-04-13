fill_rectangles (void			*_dst,
		 cairo_operator_t	 op,
		 const cairo_color_t	*color,
		 cairo_rectangle_int_t	*rects,
		 int			 num_rects)
{
    cairo_image_surface_t *dst = _dst;
    uint32_t pixel;
    int i;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (fill_reduces_to_source (op, color, dst, &pixel)) {
	for (i = 0; i < num_rects; i++) {
	    pixman_fill ((uint32_t *) dst->data, dst->stride / sizeof (uint32_t),
			 PIXMAN_FORMAT_BPP (dst->pixman_format),
			 rects[i].x, rects[i].y,
			 rects[i].width, rects[i].height,
			 pixel);
	}
    } else {
	pixman_image_t *src = _pixman_image_for_color (color);
	if (unlikely (src == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);

	op = _pixman_operator (op);
	for (i = 0; i < num_rects; i++) {
	    pixman_image_composite32 (op,
				      src, NULL, dst->pixman_image,
				      0, 0,
				      0, 0,
				      rects[i].x, rects[i].y,
				      rects[i].width, rects[i].height);
	}

	pixman_image_unref (src);
    }

    return CAIRO_STATUS_SUCCESS;
}