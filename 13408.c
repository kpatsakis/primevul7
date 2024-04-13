_cairo_image_bounded_spans (void *abstract_renderer,
			    int y, int height,
			    const cairo_half_open_span_t *spans,
			    unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    do {
	if (spans[0].coverage) {
	    pixman_image_compositor_blt (r->compositor,
					 spans[0].x, y,
					 spans[1].x - spans[0].x, height,
					 r->opacity * spans[0].coverage);
	}
	spans++;
    } while (--num_spans > 1);

    return CAIRO_STATUS_SUCCESS;
}