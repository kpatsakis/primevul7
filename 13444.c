mono_renderer_init (cairo_image_span_renderer_t	*r,
		    const cairo_composite_rectangles_t *composite,
		    cairo_antialias_t			 antialias,
		    cairo_bool_t			 needs_clip)
{
    cairo_image_surface_t *dst = (cairo_image_surface_t *)composite->surface;

    if (antialias != CAIRO_ANTIALIAS_NONE)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (!_cairo_pattern_is_opaque_solid (&composite->mask_pattern.base))
	return CAIRO_INT_STATUS_UNSUPPORTED;

    r->base.render_rows = NULL;
    if (composite->source_pattern.base.type == CAIRO_PATTERN_TYPE_SOLID) {
	const cairo_color_t *color;

	color = &composite->source_pattern.solid.color;
	if (composite->op == CAIRO_OPERATOR_CLEAR)
	    color = CAIRO_COLOR_TRANSPARENT;

	if (fill_reduces_to_source (composite->op, color, dst, &r->u.fill.pixel)) {
	    /* Use plain C for the fill operations as the span length is
	     * typically small, too small to payback the startup overheads of
	     * using SSE2 etc.
	     */
	    switch (PIXMAN_FORMAT_BPP(dst->pixman_format)) {
	    case 8: r->base.render_rows = _fill8_spans; break;
	    case 16: r->base.render_rows = _fill16_spans; break;
	    case 32: r->base.render_rows = _fill32_spans; break;
	    default: break;
	    }
	    r->u.fill.data = dst->data;
	    r->u.fill.stride = dst->stride;
	}
    } else if ((composite->op == CAIRO_OPERATOR_SOURCE ||
		(composite->op == CAIRO_OPERATOR_OVER &&
		 (dst->base.is_clear || (dst->base.content & CAIRO_CONTENT_ALPHA) == 0))) &&
	       composite->source_pattern.base.type == CAIRO_PATTERN_TYPE_SURFACE &&
	       composite->source_pattern.surface.surface->backend->type == CAIRO_SURFACE_TYPE_IMAGE &&
	       to_image_surface(composite->source_pattern.surface.surface)->format == dst->format)
    {
       cairo_image_surface_t *src =
	   to_image_surface(composite->source_pattern.surface.surface);
       int tx, ty;

	if (_cairo_matrix_is_integer_translation(&composite->source_pattern.base.matrix,
						 &tx, &ty) &&
	    composite->bounded.x + tx >= 0 &&
	    composite->bounded.y + ty >= 0 &&
	    composite->bounded.x + composite->bounded.width +  tx <= src->width &&
	    composite->bounded.y + composite->bounded.height + ty <= src->height) {

	    r->u.blit.stride = dst->stride;
	    r->u.blit.data = dst->data;
	    r->u.blit.src_stride = src->stride;
	    r->u.blit.src_data = src->data + src->stride * ty + tx * 4;
	    r->base.render_rows = _blit_spans;
	}
    }

    if (r->base.render_rows == NULL) {
	r->src = _pixman_image_for_pattern (dst, &composite->source_pattern.base, FALSE,
					    &composite->unbounded,
					    &composite->source_sample_area,
					    &r->u.composite.src_x, &r->u.composite.src_y);
	if (unlikely (r->src == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);

	r->u.composite.dst = to_pixman_image (composite->surface);
	r->op = _pixman_operator (composite->op);
	if (composite->is_bounded == 0) {
	    r->base.render_rows = _mono_unbounded_spans;
	    r->base.finish = _mono_finish_unbounded_spans;
	    r->u.composite.mask_y = composite->unbounded.y;
	} else
	    r->base.render_rows = _mono_spans;
    }
    r->bpp = PIXMAN_FORMAT_BPP(dst->pixman_format);

    return CAIRO_INT_STATUS_SUCCESS;
}