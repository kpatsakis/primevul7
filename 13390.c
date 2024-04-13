span_renderer_init (cairo_abstract_span_renderer_t	*_r,
		    const cairo_composite_rectangles_t *composite,
		    cairo_bool_t			 needs_clip)
{
    cairo_image_span_renderer_t *r = (cairo_image_span_renderer_t *)_r;
    cairo_image_surface_t *dst = (cairo_image_surface_t *)composite->surface;
    const cairo_pattern_t *source = &composite->source_pattern.base;
    cairo_operator_t op = composite->op;
    int src_x, src_y;
    int mask_x, mask_y;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (op == CAIRO_OPERATOR_CLEAR) {
	op = PIXMAN_OP_LERP_CLEAR;
    } else if (dst->base.is_clear &&
	       (op == CAIRO_OPERATOR_SOURCE ||
		op == CAIRO_OPERATOR_OVER ||
		op == CAIRO_OPERATOR_ADD)) {
	op = PIXMAN_OP_SRC;
    } else if (op == CAIRO_OPERATOR_SOURCE) {
	op = PIXMAN_OP_LERP_SRC;
    } else {
	op = _pixman_operator (op);
    }

    r->compositor = NULL;
    r->mask = NULL;
    r->src = _pixman_image_for_pattern (dst, source, FALSE,
					&composite->unbounded,
					&composite->source_sample_area,
					&src_x, &src_y);
    if (unlikely (r->src == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    r->opacity = 1.0;
    if (composite->mask_pattern.base.type == CAIRO_PATTERN_TYPE_SOLID) {
	r->opacity = composite->mask_pattern.solid.color.alpha;
    } else {
	r->mask = _pixman_image_for_pattern (dst,
					     &composite->mask_pattern.base,
					     TRUE,
					     &composite->unbounded,
					     &composite->mask_sample_area,
					     &mask_x, &mask_y);
	if (unlikely (r->mask == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);

	/* XXX Component-alpha? */
	if ((dst->base.content & CAIRO_CONTENT_COLOR) == 0 &&
	    _cairo_pattern_is_opaque (source, &composite->source_sample_area))
	{
	    pixman_image_unref (r->src);
	    r->src = r->mask;
	    src_x = mask_x;
	    src_y = mask_y;
	    r->mask = NULL;
	}
    }

    if (composite->is_bounded) {
	if (r->opacity == 1.)
	    r->base.render_rows = _cairo_image_bounded_opaque_spans;
	else
	    r->base.render_rows = _cairo_image_bounded_spans;
	r->base.finish = NULL;
    } else {
	if (needs_clip)
	    r->base.render_rows = _cairo_image_clipped_spans;
	else
	    r->base.render_rows = _cairo_image_unbounded_spans;
        r->base.finish = _cairo_image_finish_unbounded_spans;
	r->extents = composite->unbounded;
	r->extents.height += r->extents.y;
    }

    r->compositor =
	pixman_image_create_compositor (op, r->src, r->mask, dst->pixman_image,
					composite->unbounded.x + src_x,
					composite->unbounded.y + src_y,
					composite->unbounded.x + mask_x,
					composite->unbounded.y + mask_y,
					composite->unbounded.x,
					composite->unbounded.y,
					composite->unbounded.width,
					composite->unbounded.height);
    if (unlikely (r->compositor == NULL))
	return CAIRO_INT_STATUS_NOTHING_TO_DO;

    return CAIRO_STATUS_SUCCESS;
}