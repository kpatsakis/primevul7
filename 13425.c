_pixman_image_add_traps (pixman_image_t *image,
			 int dst_x, int dst_y,
			 cairo_traps_t *traps)
{
    cairo_trapezoid_t *t = traps->traps;
    int num_traps = traps->num_traps;
    while (num_traps--) {
	pixman_trapezoid_t trap;

	/* top/bottom will be clamped to surface bounds */
	trap.top = _cairo_fixed_to_16_16 (t->top);
	trap.bottom = _cairo_fixed_to_16_16 (t->bottom);

	/* However, all the other coordinates will have been left untouched so
	 * as not to introduce numerical error. Recompute them if they
	 * exceed the 16.16 limits.
	 */
	if (unlikely (line_exceeds_16_16 (&t->left))) {
	    project_line_x_onto_16_16 (&t->left, t->top, t->bottom, &trap.left);
	    trap.left.p1.y = trap.top;
	    trap.left.p2.y = trap.bottom;
	} else {
	    trap.left.p1.x = _cairo_fixed_to_16_16 (t->left.p1.x);
	    trap.left.p1.y = _cairo_fixed_to_16_16 (t->left.p1.y);
	    trap.left.p2.x = _cairo_fixed_to_16_16 (t->left.p2.x);
	    trap.left.p2.y = _cairo_fixed_to_16_16 (t->left.p2.y);
	}

	if (unlikely (line_exceeds_16_16 (&t->right))) {
	    project_line_x_onto_16_16 (&t->right, t->top, t->bottom, &trap.right);
	    trap.right.p1.y = trap.top;
	    trap.right.p2.y = trap.bottom;
	} else {
	    trap.right.p1.x = _cairo_fixed_to_16_16 (t->right.p1.x);
	    trap.right.p1.y = _cairo_fixed_to_16_16 (t->right.p1.y);
	    trap.right.p2.x = _cairo_fixed_to_16_16 (t->right.p2.x);
	    trap.right.p2.y = _cairo_fixed_to_16_16 (t->right.p2.y);
	}

	pixman_rasterize_trapezoid (image, &trap, -dst_x, -dst_y);
	t++;
    }
}