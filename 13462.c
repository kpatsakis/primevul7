__fill_reduces_to_source (cairo_operator_t op,
			  const cairo_color_t *color,
			  const cairo_image_surface_t *dst)
{
    if (op == CAIRO_OPERATOR_SOURCE || op == CAIRO_OPERATOR_CLEAR)
	return TRUE;
    if (op == CAIRO_OPERATOR_OVER && CAIRO_COLOR_IS_OPAQUE (color))
	return TRUE;
    if (dst->base.is_clear)
	return op == CAIRO_OPERATOR_OVER || op == CAIRO_OPERATOR_ADD;

    return FALSE;
}