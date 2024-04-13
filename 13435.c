composite_glyphs (void				*_dst,
		  cairo_operator_t		 op,
		  cairo_surface_t		*_src,
		  int				 src_x,
		  int				 src_y,
		  int				 dst_x,
		  int				 dst_y,
		  cairo_composite_glyphs_info_t *info)
{
    cairo_int_status_t status = CAIRO_INT_STATUS_SUCCESS;
    pixman_glyph_cache_t *glyph_cache;
    pixman_glyph_t pglyphs_stack[CAIRO_STACK_ARRAY_LENGTH (pixman_glyph_t)];
    pixman_glyph_t *pglyphs = pglyphs_stack;
    pixman_glyph_t *pg;
    int i;

    TRACE ((stderr, "%s\n", __FUNCTION__));

    CAIRO_MUTEX_LOCK (_cairo_glyph_cache_mutex);

    glyph_cache = get_glyph_cache();
    if (unlikely (glyph_cache == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto out_unlock;
    }

    pixman_glyph_cache_freeze (glyph_cache);

    if (info->num_glyphs > ARRAY_LENGTH (pglyphs_stack)) {
	pglyphs = _cairo_malloc_ab (info->num_glyphs, sizeof (pixman_glyph_t));
	if (unlikely (pglyphs == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto out_thaw;
	}
    }

    pg = pglyphs;
    for (i = 0; i < info->num_glyphs; i++) {
	unsigned long index = info->glyphs[i].index;
	const void *glyph;
        int xphase, yphase;

        xphase = PHASE(info->glyphs[i].x);
        yphase = PHASE(info->glyphs[i].y);

	index = index | (xphase << 24) | (yphase << 26);

	glyph = pixman_glyph_cache_lookup (glyph_cache, info->font, (void *)index);
	if (!glyph) {
	    cairo_scaled_glyph_t *scaled_glyph;
	    cairo_image_surface_t *glyph_surface;

	    /* This call can actually end up recursing, so we have to
	     * drop the mutex around it.
	     */
	    CAIRO_MUTEX_UNLOCK (_cairo_glyph_cache_mutex);
	    status = _cairo_scaled_glyph_lookup (info->font, index,
						 CAIRO_SCALED_GLYPH_INFO_SURFACE,
						 &scaled_glyph);
	    CAIRO_MUTEX_LOCK (_cairo_glyph_cache_mutex);

	    if (unlikely (status))
		goto out_thaw;

	    glyph_surface = scaled_glyph->surface;
	    glyph = pixman_glyph_cache_insert (glyph_cache, info->font, (void *)index,
					       glyph_surface->base.device_transform.x0,
					       glyph_surface->base.device_transform.y0,
					       glyph_surface->pixman_image);
	    if (unlikely (!glyph)) {
		status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
		goto out_thaw;
	    }
	}

	pg->x = POSITION (info->glyphs[i].x);
	pg->y = POSITION (info->glyphs[i].y);
	pg->glyph = glyph;
	pg++;
    }

    if (info->use_mask) {
	pixman_format_code_t mask_format;

	mask_format = pixman_glyph_get_mask_format (glyph_cache, pg - pglyphs, pglyphs);

	pixman_composite_glyphs (_pixman_operator (op),
				 ((cairo_image_source_t *)_src)->pixman_image,
				 to_pixman_image (_dst),
				 mask_format,
				 info->extents.x + src_x, info->extents.y + src_y,
				 info->extents.x, info->extents.y,
				 info->extents.x - dst_x, info->extents.y - dst_y,
				 info->extents.width, info->extents.height,
				 glyph_cache, pg - pglyphs, pglyphs);
    } else {
	pixman_composite_glyphs_no_mask (_pixman_operator (op),
					 ((cairo_image_source_t *)_src)->pixman_image,
					 to_pixman_image (_dst),
					 src_x, src_y,
					 - dst_x, - dst_y,
					 glyph_cache, pg - pglyphs, pglyphs);
    }

out_thaw:
    pixman_glyph_cache_thaw (glyph_cache);

    if (pglyphs != pglyphs_stack)
	free(pglyphs);

out_unlock:
    CAIRO_MUTEX_UNLOCK (_cairo_glyph_cache_mutex);
    return status;
}