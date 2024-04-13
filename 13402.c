_mono_finish_unbounded_spans (void *abstract_renderer)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (r->u.composite.mask_y < r->composite->unbounded.y + r->composite->unbounded.height) {
	pixman_image_composite32 (PIXMAN_OP_CLEAR,
				  r->src, NULL, r->u.composite.dst,
				  r->composite->unbounded.x + r->u.composite.src_x,  r->u.composite.mask_y + r->u.composite.src_y,
				  0, 0,
				  r->composite->unbounded.x, r->u.composite.mask_y,
				  r->composite->unbounded.width,
				  r->composite->unbounded.y + r->composite->unbounded.height - r->u.composite.mask_y);
    }

    return CAIRO_STATUS_SUCCESS;
}