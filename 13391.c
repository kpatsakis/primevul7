draw_image_boxes (void *_dst,
		  cairo_image_surface_t *image,
		  cairo_boxes_t *boxes,
		  int dx, int dy)
{
    cairo_image_surface_t *dst = _dst;
    struct _cairo_boxes_chunk *chunk;
    int i;

    TRACE ((stderr, "%s x %d\n", __FUNCTION__, boxes->num_boxes));

    for (chunk = &boxes->chunks; chunk; chunk = chunk->next) {
	for (i = 0; i < chunk->count; i++) {
	    cairo_box_t *b = &chunk->base[i];
	    int x = _cairo_fixed_integer_part (b->p1.x);
	    int y = _cairo_fixed_integer_part (b->p1.y);
	    int w = _cairo_fixed_integer_part (b->p2.x) - x;
	    int h = _cairo_fixed_integer_part (b->p2.y) - y;
	    if (dst->pixman_format != image->pixman_format ||
		! pixman_blt ((uint32_t *)image->data, (uint32_t *)dst->data,
			      image->stride / sizeof (uint32_t),
			      dst->stride / sizeof (uint32_t),
			      PIXMAN_FORMAT_BPP (image->pixman_format),
			      PIXMAN_FORMAT_BPP (dst->pixman_format),
			      x + dx, y + dy,
			      x, y,
			      w, h))
	    {
		pixman_image_composite32 (PIXMAN_OP_SRC,
					  image->pixman_image, NULL, dst->pixman_image,
					  x + dx, y + dy,
					  0, 0,
					  x, y,
					  w, h);
	    }
	}
    }
    return CAIRO_STATUS_SUCCESS;
}