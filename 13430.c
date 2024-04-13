_inplace_opacity_spans (void *abstract_renderer, int y, int h,
			const cairo_half_open_span_t *spans,
			unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;
    uint8_t *mask;
    int x0, x1;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    mask = (uint8_t *)pixman_image_get_data (r->mask);
    x1 = x0 = spans[0].x;
    do {
	int len = spans[1].x - spans[0].x;
	uint8_t m = mul8_8(spans[0].coverage, r->bpp);
	*mask++ = m;
	if (len > 1) {
	    if (m == 0 &&
		x1 - x0 > r->u.composite.run_length) {
		pixman_image_composite32 (r->op, r->src, r->mask, r->u.composite.dst,
					  x0 + r->u.composite.src_x,
					  y + r->u.composite.src_y,
					  0, 0,
					  x0, y,
					  x1 - x0, h);
		mask = (uint8_t *)pixman_image_get_data (r->mask);
		x0 = spans[1].x;
	    }else {
		memset (mask, m, --len);
		mask += len;
	    }
	}
	x1 = spans[1].x;
	spans++;
    } while (--num_spans > 1);

    if (x1 != x0) {
	pixman_image_composite32 (r->op, r->src, r->mask, r->u.composite.dst,
				  x0 + r->u.composite.src_x,
				  y + r->u.composite.src_y,
				  0, 0,
				  x0, y,
				  x1 - x0, h);
    }

    return CAIRO_STATUS_SUCCESS;
}