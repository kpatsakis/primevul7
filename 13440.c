_inplace_src_opacity_spans (void *abstract_renderer, int y, int h,
			    const cairo_half_open_span_t *spans,
			    unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;
    uint8_t *mask;
    int x0;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    x0 = spans[0].x;
    mask = (uint8_t *)pixman_image_get_data (r->mask);
    do {
	int len = spans[1].x - spans[0].x;
	uint8_t m = mul8_8(spans[0].coverage, r->bpp);
	if (m == 0) {
	    if (spans[0].x != x0) {
#if PIXMAN_HAS_OP_LERP
		pixman_image_composite32 (PIXMAN_OP_LERP_SRC,
					  r->src, r->mask, r->u.composite.dst,
					  x0 + r->u.composite.src_x,
					  y + r->u.composite.src_y,
					  0, 0,
					  x0, y,
					  spans[0].x - x0, h);
#else
		pixman_image_composite32 (PIXMAN_OP_OUT_REVERSE,
					  r->mask, NULL, r->u.composite.dst,
					  0, 0,
					  0, 0,
					  x0, y,
					  spans[0].x - x0, h);
		pixman_image_composite32 (PIXMAN_OP_ADD,
					  r->src, r->mask, r->u.composite.dst,
					  x0 + r->u.composite.src_x,
					  y + r->u.composite.src_y,
					  0, 0,
					  x0, y,
					  spans[0].x - x0, h);
#endif
	    }

	    mask = (uint8_t *)pixman_image_get_data (r->mask);
	    x0 = spans[1].x;
	} else {
	    *mask++ = m;
	    if (len > 1) {
		memset (mask, m, --len);
		mask += len;
	    }
	}
	spans++;
    } while (--num_spans > 1);

    if (spans[0].x != x0) {
#if PIXMAN_HAS_OP_LERP
	pixman_image_composite32 (PIXMAN_OP_LERP_SRC,
				  r->src, r->mask, r->u.composite.dst,
				  x0 + r->u.composite.src_x,
				  y + r->u.composite.src_y,
				  0, 0,
				  x0, y,
				  spans[0].x - x0, h);
#else
	pixman_image_composite32 (PIXMAN_OP_OUT_REVERSE,
				  r->mask, NULL, r->u.composite.dst,
				  0, 0,
				  0, 0,
				  x0, y,
				  spans[0].x - x0, h);
	pixman_image_composite32 (PIXMAN_OP_ADD,
				  r->src, r->mask, r->u.composite.dst,
				  x0 + r->u.composite.src_x,
				  y + r->u.composite.src_y,
				  0, 0,
				  x0, y,
				  spans[0].x - x0, h);
#endif
    }

    return CAIRO_STATUS_SUCCESS;
}