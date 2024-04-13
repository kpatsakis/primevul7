_fill32_spans (void *abstract_renderer, int y, int h,
	       const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    if (likely(h == 1)) {
	do {
	    if (spans[0].coverage) {
		int len = spans[1].x - spans[0].x;
		if (len > 32) {
		    pixman_fill ((uint32_t *)r->u.fill.data, r->u.fill.stride / sizeof(uint32_t), r->bpp,
				 spans[0].x, y, len, 1, r->u.fill.pixel);
		} else {
		    uint32_t *d = (uint32_t*)(r->u.fill.data + r->u.fill.stride*y + spans[0].x*4);
		    while (len-- > 0)
			*d++ = r->u.fill.pixel;
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    } else {
	do {
	    if (spans[0].coverage) {
		if (spans[1].x - spans[0].x > 16) {
		    pixman_fill ((uint32_t *)r->u.fill.data, r->u.fill.stride / sizeof(uint32_t), r->bpp,
				 spans[0].x, y, spans[1].x - spans[0].x, h,
				 r->u.fill.pixel);
		} else {
		    int yy = y, hh = h;
		    do {
			int len = spans[1].x - spans[0].x;
			uint32_t *d = (uint32_t*)(r->u.fill.data + r->u.fill.stride*yy + spans[0].x*4);
			while (len-- > 0)
			    *d++ = r->u.fill.pixel;
			yy++;
		    } while (--hh);
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    }

    return CAIRO_STATUS_SUCCESS;
}