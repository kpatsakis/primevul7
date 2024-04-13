_cairo_image_unbounded_spans (void *abstract_renderer,
			      int y, int height,
			      const cairo_half_open_span_t *spans,
			      unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;

    assert (y + height <= r->extents.height);
    if (y > r->extents.y) {
	pixman_image_compositor_blt (r->compositor,
				     r->extents.x, r->extents.y,
				     r->extents.width, y - r->extents.y,
				     0);
    }

    if (num_spans == 0) {
	pixman_image_compositor_blt (r->compositor,
				     r->extents.x, y,
				     r->extents.width,  height,
				     0);
    } else {
	if (spans[0].x != r->extents.x) {
	    pixman_image_compositor_blt (r->compositor,
					 r->extents.x, y,
					 spans[0].x - r->extents.x,
					 height,
					 0);
	}

	do {
	    assert (spans[0].x < r->extents.x + r->extents.width);
	    pixman_image_compositor_blt (r->compositor,
					 spans[0].x, y,
					 spans[1].x - spans[0].x, height,
					 r->opacity * spans[0].coverage);
	    spans++;
	} while (--num_spans > 1);

	if (spans[0].x != r->extents.x + r->extents.width) {
	    assert (spans[0].x < r->extents.x + r->extents.width);
	    pixman_image_compositor_blt (r->compositor,
					 spans[0].x,     y,
					 r->extents.x + r->extents.width - spans[0].x, height,
					 0);
	}
    }

    r->extents.y = y + height;
    return CAIRO_STATUS_SUCCESS;
}