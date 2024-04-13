_mono_unbounded_spans (void *abstract_renderer, int y, int h,
		       const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    if (num_spans == 0) {
	pixman_image_composite32 (PIXMAN_OP_CLEAR,
				  r->src, NULL, r->u.composite.dst,
				  spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				  0, 0,
				  r->composite->unbounded.x, y,
				  r->composite->unbounded.width, h);
	r->u.composite.mask_y = y + h;
	return CAIRO_STATUS_SUCCESS;
    }

    if (y != r->u.composite.mask_y) {
	pixman_image_composite32 (PIXMAN_OP_CLEAR,
				  r->src, NULL, r->u.composite.dst,
				  spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				  0, 0,
				  r->composite->unbounded.x, r->u.composite.mask_y,
				  r->composite->unbounded.width, y - r->u.composite.mask_y);
    }

    if (spans[0].x != r->composite->unbounded.x) {
	    pixman_image_composite32 (PIXMAN_OP_CLEAR,
				      r->src, NULL, r->u.composite.dst,
				      spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				      0, 0,
				      r->composite->unbounded.x, y,
				      spans[0].x - r->composite->unbounded.x, h);
    }

    do {
	int op = spans[0].coverage ? r->op : PIXMAN_OP_CLEAR;
	pixman_image_composite32 (op,
				  r->src, NULL, r->u.composite.dst,
				  spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				  0, 0,
				  spans[0].x, y,
				  spans[1].x - spans[0].x, h);
	spans++;
    } while (--num_spans > 1);

    if (spans[0].x != r->composite->unbounded.x + r->composite->unbounded.width) {
	    pixman_image_composite32 (PIXMAN_OP_CLEAR,
				      r->src, NULL, r->u.composite.dst,
				      spans[0].x + r->u.composite.src_x,  y + r->u.composite.src_y,
				      0, 0,
				      spans[0].x, y,
				      r->composite->unbounded.x + r->composite->unbounded.width - spans[0].x, h);
    }

    r->u.composite.mask_y = y + h;
    return CAIRO_STATUS_SUCCESS;
}