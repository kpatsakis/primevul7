_cairo_image_finish_spans_and_zero (void *abstract_renderer)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (r->u.mask.extents.y < r->u.mask.extents.height)
	memset (r->u.mask.data, 0, (r->u.mask.extents.height - r->u.mask.extents.y) * r->u.mask.stride);

    return CAIRO_STATUS_SUCCESS;
}