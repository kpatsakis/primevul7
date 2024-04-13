_fill8_spans (void *abstract_renderer, int y, int h,
	       const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    if (likely(h == 1)) {
	do {
	    if (spans[0].coverage) {
		int len = spans[1].x - spans[0].x;
		uint8_t *d = r->u.fill.data + r->u.fill.stride*y + spans[0].x;
		if (len == 1)
		    *d = r->u.fill.pixel;
		else
		    memset(d, r->u.fill.pixel, len);
	    }
	    spans++;
	} while (--num_spans > 1);
    } else {
	do {
	    if (spans[0].coverage) {
		int yy = y, hh = h;
		do {
		    int len = spans[1].x - spans[0].x;
		    uint8_t *d = r->u.fill.data + r->u.fill.stride*yy + spans[0].x;
		    if (len == 1)
			*d = r->u.fill.pixel;
		    else
			memset(d, r->u.fill.pixel, len);
		    yy++;
		} while (--hh);
	    }
	    spans++;
	} while (--num_spans > 1);
    }

    return CAIRO_STATUS_SUCCESS;
}