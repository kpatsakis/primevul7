_cairo_image_finish_unbounded_spans (void *abstract_renderer)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (r->extents.y < r->extents.height) {
	pixman_image_compositor_blt (r->compositor,
				     r->extents.x, r->extents.y,
				     r->extents.width,
				     r->extents.height - r->extents.y,
				     0);
    }

    return CAIRO_STATUS_SUCCESS;
}