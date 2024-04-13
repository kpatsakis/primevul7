_fill_xrgb32_lerp_opaque_spans (void *abstract_renderer, int y, int h,
				const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    if (likely(h == 1)) {
	do {
	    uint8_t a = spans[0].coverage;
	    if (a) {
		int len = spans[1].x - spans[0].x;
		uint32_t *d = (uint32_t*)(r->u.fill.data + r->u.fill.stride*y + spans[0].x*4);
		if (a == 0xff) {
		    if (len > 31) {
			pixman_fill ((uint32_t *)r->u.fill.data, r->u.fill.stride / sizeof(uint32_t), 32,
				     spans[0].x, y, len, 1, r->u.fill.pixel);
		    } else {
			uint32_t *d = (uint32_t*)(r->u.fill.data + r->u.fill.stride*y + spans[0].x*4);
			while (len-- > 0)
			    *d++ = r->u.fill.pixel;
		    }
		} else while (len-- > 0) {
		    *d = lerp8x4 (r->u.fill.pixel, a, *d);
		    d++;
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    } else {
	do {
	    uint8_t a = spans[0].coverage;
	    if (a) {
		if (a == 0xff) {
		    if (spans[1].x - spans[0].x > 16) {
			pixman_fill ((uint32_t *)r->u.fill.data, r->u.fill.stride / sizeof(uint32_t), 32,
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
		} else {
		    int yy = y, hh = h;
		    do {
			int len = spans[1].x - spans[0].x;
			uint32_t *d = (uint32_t *)(r->u.fill.data + r->u.fill.stride*yy + spans[0].x*4);
			while (len-- > 0) {
			    *d = lerp8x4 (r->u.fill.pixel, a, *d);
			    d++;
			}
			yy++;
		    } while (--hh);
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    }

    return CAIRO_STATUS_SUCCESS;
}