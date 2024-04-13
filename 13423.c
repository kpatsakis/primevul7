fill_reduces_to_source (cairo_operator_t op,
			const cairo_color_t *color,
			const cairo_image_surface_t *dst,
			uint32_t *pixel)
{
    if (__fill_reduces_to_source (op, color, dst)) {
	return color_to_pixel (color, dst->pixman_format, pixel);
    }

    return FALSE;
}