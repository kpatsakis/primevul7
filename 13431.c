_cairo_image_spans (void *abstract_renderer,
		    int y, int height,
		    const cairo_half_open_span_t *spans,
		    unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;
    uint8_t *mask, *row;
    int len;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    mask = r->u.mask.data + (y - r->u.mask.extents.y) * r->u.mask.stride;
    mask += spans[0].x - r->u.mask.extents.x;
    row = mask;

    do {
	len = spans[1].x - spans[0].x;
	if (spans[0].coverage) {
	    *row++ = r->opacity * spans[0].coverage;
	    if (--len)
		memset (row, row[-1], len);
	}
	row += len;
	spans++;
    } while (--num_spans > 1);

    len = row - mask;
    row = mask;
    while (--height) {
	mask += r->u.mask.stride;
	memcpy (mask, row, len);
    }

    return CAIRO_STATUS_SUCCESS;
}