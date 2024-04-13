_fill_a8_lerp_opaque_spans (void *abstract_renderer, int y, int h,
			    const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    if (likely(h == 1)) {
	uint8_t *d = r->u.fill.data + r->u.fill.stride*y;
	do {
	    uint8_t a = spans[0].coverage;
	    if (a) {
		int len = spans[1].x - spans[0].x;
		if (a == 0xff) {
		    memset(d + spans[0].x, r->u.fill.pixel, len);
		} else {
		    uint8_t s = mul8_8(a, r->u.fill.pixel);
		    uint8_t *dst = d + spans[0].x;
		    a = ~a;
		    while (len-- > 0) {
			uint8_t t = mul8_8(*dst, a);
			*dst++ = t + s;
		    }
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    } else {
	do {
	    uint8_t a = spans[0].coverage;
	    if (a) {
		int yy = y, hh = h;
		if (a == 0xff) {
		    do {
			int len = spans[1].x - spans[0].x;
			uint8_t *d = r->u.fill.data + r->u.fill.stride*yy + spans[0].x;
			memset(d, r->u.fill.pixel, len);
			yy++;
		    } while (--hh);
		} else {
		    uint8_t s = mul8_8(a, r->u.fill.pixel);
		    a = ~a;
		    do {
			int len = spans[1].x - spans[0].x;
			uint8_t *d = r->u.fill.data + r->u.fill.stride*yy + spans[0].x;
			while (len-- > 0) {
			    uint8_t t = mul8_8(*d, a);
			    *d++ = t + s;
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