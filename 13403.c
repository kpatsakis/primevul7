span_renderer_fini (cairo_abstract_span_renderer_t *_r,
		    cairo_int_status_t status)
{
    cairo_image_span_renderer_t *r = (cairo_image_span_renderer_t *) _r;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (likely (status == CAIRO_INT_STATUS_SUCCESS)) {
	if (r->base.finish)
	    r->base.finish (r);
    }
    if (likely (status == CAIRO_INT_STATUS_SUCCESS && r->bpp == 0)) {
	const cairo_composite_rectangles_t *composite = r->composite;

	pixman_image_composite32 (r->op, r->src, r->mask,
				  to_pixman_image (composite->surface),
				  composite->unbounded.x + r->u.mask.src_x,
				  composite->unbounded.y + r->u.mask.src_y,
				  0, 0,
				  composite->unbounded.x,
				  composite->unbounded.y,
				  composite->unbounded.width,
				  composite->unbounded.height);
    }

    if (r->src)
	pixman_image_unref (r->src);
    if (r->mask)
	pixman_image_unref (r->mask);
}