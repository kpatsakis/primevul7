fill_boxes (void		*_dst,
	    cairo_operator_t	 op,
	    const cairo_color_t	*color,
	    cairo_boxes_t	*boxes)
{
    cairo_image_surface_t *dst = _dst;
    struct _cairo_boxes_chunk *chunk;
    uint32_t pixel;
    int i;

    TRACE ((stderr, "%s x %d\n", __FUNCTION__, boxes->num_boxes));

    if (fill_reduces_to_source (op, color, dst, &pixel)) {
	for (chunk = &boxes->chunks; chunk; chunk = chunk->next) {
	    for (i = 0; i < chunk->count; i++) {
		int x = _cairo_fixed_integer_part (chunk->base[i].p1.x);
		int y = _cairo_fixed_integer_part (chunk->base[i].p1.y);
		int w = _cairo_fixed_integer_part (chunk->base[i].p2.x) - x;
		int h = _cairo_fixed_integer_part (chunk->base[i].p2.y) - y;
		pixman_fill ((uint32_t *) dst->data,
			     dst->stride / sizeof (uint32_t),
			     PIXMAN_FORMAT_BPP (dst->pixman_format),
			     x, y, w, h, pixel);
	    }
	}
    }
    else
    {
	pixman_image_t *src = _pixman_image_for_color (color);
	if (unlikely (src == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);

	op = _pixman_operator (op);
	for (chunk = &boxes->chunks; chunk; chunk = chunk->next) {
	    for (i = 0; i < chunk->count; i++) {
		int x1 = _cairo_fixed_integer_part (chunk->base[i].p1.x);
		int y1 = _cairo_fixed_integer_part (chunk->base[i].p1.y);
		int x2 = _cairo_fixed_integer_part (chunk->base[i].p2.x);
		int y2 = _cairo_fixed_integer_part (chunk->base[i].p2.y);
		pixman_image_composite32 (op,
					  src, NULL, dst->pixman_image,
					  0, 0,
					  0, 0,
					  x1, y1,
					  x2-x1, y2-y1);
	    }
	}

	pixman_image_unref (src);
    }

    return CAIRO_STATUS_SUCCESS;
}