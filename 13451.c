composite_boxes (void			*_dst,
		 cairo_operator_t	op,
		 cairo_surface_t	*abstract_src,
		 cairo_surface_t	*abstract_mask,
		 int			src_x,
		 int			src_y,
		 int			mask_x,
		 int			mask_y,
		 int			dst_x,
		 int			dst_y,
		 cairo_boxes_t		*boxes,
		 const cairo_rectangle_int_t  *extents)
{
    pixman_image_t *dst = to_pixman_image (_dst);
    pixman_image_t *src = ((cairo_image_source_t *)abstract_src)->pixman_image;
    pixman_image_t *mask = abstract_mask ? ((cairo_image_source_t *)abstract_mask)->pixman_image : NULL;
    pixman_image_t *free_src = NULL;
    struct _cairo_boxes_chunk *chunk;
    int i;

    /* XXX consider using a region? saves multiple prepare-composite */
    TRACE ((stderr, "%s x %d\n", __FUNCTION__, boxes->num_boxes));

    if (((cairo_surface_t *)_dst)->is_clear &&
	(op == CAIRO_OPERATOR_SOURCE ||
	 op == CAIRO_OPERATOR_OVER ||
	 op == CAIRO_OPERATOR_ADD)) {
	op = PIXMAN_OP_SRC;
    } else if (mask) {
	if (op == CAIRO_OPERATOR_CLEAR) {
#if PIXMAN_HAS_OP_LERP
	    op = PIXMAN_OP_LERP_CLEAR;
#else
	    free_src = src = _pixman_image_for_color (CAIRO_COLOR_WHITE);
	    if (unlikely (src == NULL))
		return _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    op = PIXMAN_OP_OUT_REVERSE;
#endif
	} else if (op == CAIRO_OPERATOR_SOURCE) {
#if PIXMAN_HAS_OP_LERP
	    op = PIXMAN_OP_LERP_SRC;
#else
	    return CAIRO_INT_STATUS_UNSUPPORTED;
#endif
	} else {
	    op = _pixman_operator (op);
	}
    } else {
	op = _pixman_operator (op);
    }

    for (chunk = &boxes->chunks; chunk; chunk = chunk->next) {
	for (i = 0; i < chunk->count; i++) {
	    int x1 = _cairo_fixed_integer_part (chunk->base[i].p1.x);
	    int y1 = _cairo_fixed_integer_part (chunk->base[i].p1.y);
	    int x2 = _cairo_fixed_integer_part (chunk->base[i].p2.x);
	    int y2 = _cairo_fixed_integer_part (chunk->base[i].p2.y);

	    pixman_image_composite32 (op, src, mask, dst,
				      x1 + src_x, y1 + src_y,
				      x1 + mask_x, y1 + mask_y,
				      x1 + dst_x, y1 + dst_y,
				      x2 - x1, y2 - y1);
	}
    }

    if (free_src)
	pixman_image_unref (free_src);

    return CAIRO_STATUS_SUCCESS;
}