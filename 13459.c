span_renderer_fini (cairo_abstract_span_renderer_t *_r,
		    cairo_int_status_t status)
{
    cairo_image_span_renderer_t *r = (cairo_image_span_renderer_t *) _r;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (status == CAIRO_INT_STATUS_SUCCESS && r->base.finish)
	r->base.finish (r);

    if (r->compositor)
	pixman_image_compositor_destroy (r->compositor);

    if (r->src)
	pixman_image_unref (r->src);
    if (r->mask)
	pixman_image_unref (r->mask);
}