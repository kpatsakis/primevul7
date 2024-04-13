_blit_spans (void *abstract_renderer, int y, int h,
	     const cairo_half_open_span_t *spans, unsigned num_spans)
{
    cairo_image_span_renderer_t *r = abstract_renderer;
    int cpp;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    cpp = r->bpp/8;
    if (likely (h == 1)) {
	uint8_t *src = r->u.blit.src_data + y*r->u.blit.src_stride;
	uint8_t *dst = r->u.blit.data + y*r->u.blit.stride;
	do {
	    if (spans[0].coverage) {
		void *s = src + spans[0].x*cpp;
		void *d = dst + spans[0].x*cpp;
		int len = (spans[1].x - spans[0].x) * cpp;
		switch (len) {
		case 1:
		    *(uint8_t *)d = *(uint8_t *)s;
		    break;
		case 2:
		    *(uint16_t *)d = *(uint16_t *)s;
		    break;
		case 4:
		    *(uint32_t *)d = *(uint32_t *)s;
		    break;
#if HAVE_UINT64_T
		case 8:
		    *(uint64_t *)d = *(uint64_t *)s;
		    break;
#endif
		default:
		    memcpy(d, s, len);
		    break;
		}
	    }
	    spans++;
	} while (--num_spans > 1);
    } else {
	do {
	    if (spans[0].coverage) {
		int yy = y, hh = h;
		do {
		    void *src = r->u.blit.src_data + yy*r->u.blit.src_stride + spans[0].x*cpp;
		    void *dst = r->u.blit.data + yy*r->u.blit.stride + spans[0].x*cpp;
		    int len = (spans[1].x - spans[0].x) * cpp;
		    switch (len) {
		    case 1:
			*(uint8_t *)dst = *(uint8_t *)src;
			break;
		    case 2:
			*(uint16_t *)dst = *(uint16_t *)src;
			break;
		    case 4:
			*(uint32_t *)dst = *(uint32_t *)src;
			break;
#if HAVE_UINT64_T
		    case 8:
			*(uint64_t *)dst = *(uint64_t *)src;
			break;
#endif
		    default:
			memcpy(dst, src, len);
			break;
		    }
		    yy++;
		} while (--hh);
	    }
	    spans++;
	} while (--num_spans > 1);
    }

    return CAIRO_STATUS_SUCCESS;
}