lerp (void			*_dst,
      cairo_surface_t		*abstract_src,
      cairo_surface_t		*abstract_mask,
      int			src_x,
      int			src_y,
      int			mask_x,
      int			mask_y,
      int			dst_x,
      int			dst_y,
      unsigned int		width,
      unsigned int		height)
{
    cairo_image_surface_t *dst = _dst;
    cairo_image_source_t *src = (cairo_image_source_t *)abstract_src;
    cairo_image_source_t *mask = (cairo_image_source_t *)abstract_mask;

    TRACE ((stderr, "%s\n", __FUNCTION__));

#if PIXMAN_HAS_OP_LERP
    pixman_image_composite32 (PIXMAN_OP_LERP_SRC,
			      src->pixman_image, mask->pixman_image, dst->pixman_image,
			      src_x,  src_y,
			      mask_x, mask_y,
			      dst_x,  dst_y,
			      width,  height);
#else
    /* Punch the clip out of the destination */
    TRACE ((stderr, "%s - OUT_REVERSE (mask=%d/%p, dst=%d/%p)\n",
	    __FUNCTION__,
	    mask->base.unique_id, mask->pixman_image,
	    dst->base.unique_id, dst->pixman_image));
    pixman_image_composite32 (PIXMAN_OP_OUT_REVERSE,
			      mask->pixman_image, NULL, dst->pixman_image,
			      mask_x, mask_y,
			      0,      0,
			      dst_x,  dst_y,
			      width,  height);

    /* Now add the two results together */
    TRACE ((stderr, "%s - ADD (src=%d/%p, mask=%d/%p, dst=%d/%p)\n",
	    __FUNCTION__,
	    src->base.unique_id, src->pixman_image,
	    mask->base.unique_id, mask->pixman_image,
	    dst->base.unique_id, dst->pixman_image));
    pixman_image_composite32 (PIXMAN_OP_ADD,
			      src->pixman_image, mask->pixman_image, dst->pixman_image,
			      src_x,  src_y,
			      mask_x, mask_y,
			      dst_x,  dst_y,
			      width,  height);
#endif

    return CAIRO_STATUS_SUCCESS;
}