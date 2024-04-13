composite (void			*_dst,
	   cairo_operator_t	op,
	   cairo_surface_t	*abstract_src,
	   cairo_surface_t	*abstract_mask,
	   int			src_x,
	   int			src_y,
	   int			mask_x,
	   int			mask_y,
	   int			dst_x,
	   int			dst_y,
	   unsigned int		width,
	   unsigned int		height)
{
    cairo_image_source_t *src = (cairo_image_source_t *)abstract_src;
    cairo_image_source_t *mask = (cairo_image_source_t *)abstract_mask;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    if (mask) {
	pixman_image_composite32 (_pixman_operator (op),
				  src->pixman_image, mask->pixman_image, to_pixman_image (_dst),
				  src_x, src_y,
				  mask_x, mask_y,
				  dst_x, dst_y,
				  width, height);
    } else {
	pixman_image_composite32 (_pixman_operator (op),
				  src->pixman_image, NULL, to_pixman_image (_dst),
				  src_x, src_y,
				  0, 0,
				  dst_x, dst_y,
				  width, height);
    }

    return CAIRO_STATUS_SUCCESS;
}