_cairo_image_clipped_spans (void *abstract_renderer,
			    int y, int height,
			    const cairo_half_open_span_t *spans,
			    unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    assert (num_spans);

    do {
	if (! spans[0].inverse)
	    pixman_image_compositor_blt (r->compositor,
					 spans[0].x, y,
					 spans[1].x - spans[0].x, height,
					 r->opacity * spans[0].coverage);
	spans++;
    } while (--num_spans > 1);

    r->extents.y = y + height;
    return CAIRO_STATUS_SUCCESS;
}