_cairo_image_spans_and_zero (void *abstract_renderer,
			     int y, int height,
			     const cairo_half_open_span_t *spans,
			     unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;
    uint8_t *mask;
    int len;

    mask = r->u.mask.data;
    if (y > r->u.mask.extents.y) {
	len = (y - r->u.mask.extents.y) * r->u.mask.stride;
	memset (mask, 0, len);
	mask += len;
    }

    r->u.mask.extents.y = y + height;
    r->u.mask.data = mask + height * r->u.mask.stride;
    if (num_spans == 0) {
	memset (mask, 0, height * r->u.mask.stride);
    } else {
	uint8_t *row = mask;

	if (spans[0].x != r->u.mask.extents.x) {
	    len = spans[0].x - r->u.mask.extents.x;
	    memset (row, 0, len);
	    row += len;
	}

	do {
	    len = spans[1].x - spans[0].x;
	    *row++ = r->opacity * spans[0].coverage;
	    if (len > 1) {
		memset (row, row[-1], --len);
		row += len;
	    }
	    spans++;
	} while (--num_spans > 1);

	if (spans[0].x != r->u.mask.extents.x + r->u.mask.extents.width) {
	    len = r->u.mask.extents.x + r->u.mask.extents.width - spans[0].x;
	    memset (row, 0, len);
	}

	row = mask;
	while (--height) {
	    mask += r->u.mask.stride;
	    memcpy (mask, row, r->u.mask.extents.width);
	}
    }

    return CAIRO_STATUS_SUCCESS;
}