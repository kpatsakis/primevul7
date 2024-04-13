_mono_spans (void *abstract_renderer, int y, int h,
	     const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    do {
	if (spans[0].coverage) {
	    pixman_image_composite32 (r->op,
				      r->src, NULL, r->u.composite.dst,
				      spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				      0, 0,
				      spans[0].x, y,
				      spans[1].x - spans[0].x, h);
	}
	spans++;
    } while (--num_spans > 1);

    return CAIRO_STATUS_SUCCESS;
}