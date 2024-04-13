fill_entry (IconEntry *icon, 
	    GdkPixbuf *pixbuf, 
	    gint       hot_x, 
	    gint       hot_y, 
	    GError   **error) 
 {
	guchar *p, *pixels, *and, *xor;
	gint n_channels, v, x, y;

	if (icon->width > 256 || icon->height > 256) {
		g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_BAD_OPTION,
                                     _("Image too large to be saved as ICO"));
		return FALSE;
	} 
	
	if (hot_x > -1 && hot_y > -1) {
		icon->hot_x = hot_x;
		icon->hot_y = hot_y;
		if (icon->hot_x >= icon->width || icon->hot_y >= icon->height) {
			g_set_error_literal (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_BAD_OPTION,
                                             _("Cursor hotspot outside image"));
			return FALSE;
		}
	}
	else {
		icon->hot_x = -1;
		icon->hot_y = -1;
	}
	
	switch (icon->depth) {
	case 32:
		icon->xor_rowstride = icon->width * 4;
		break;
	case 24:
		icon->xor_rowstride = icon->width * 3;
		break;
	case 16:
		icon->xor_rowstride = icon->width * 2;
		break;
	default:
		g_set_error (error,
			     GDK_PIXBUF_ERROR,
			     GDK_PIXBUF_ERROR_BAD_OPTION,
			     _("Unsupported depth for ICO file: %d"), icon->depth);
		return FALSE;
	}

	if ((icon->xor_rowstride % 4) != 0)
		icon->xor_rowstride = 4 * ((icon->xor_rowstride / 4) + 1);
	icon->xor = g_new0 (guchar, icon->xor_rowstride * icon->height);

	icon->and_rowstride = (icon->width + 7) / 8;
	if ((icon->and_rowstride % 4) != 0)
		icon->and_rowstride = 4 * ((icon->and_rowstride / 4) + 1);
	icon->and = g_new0 (guchar, icon->and_rowstride * icon->height);

	pixels = gdk_pixbuf_get_pixels (pixbuf);
	n_channels = gdk_pixbuf_get_n_channels (pixbuf);
	for (y = 0; y < icon->height; y++) {
		p = pixels + (gsize) gdk_pixbuf_get_rowstride (pixbuf) * (icon->height - 1 - y);
		and = icon->and + icon->and_rowstride * y;
		xor = icon->xor + icon->xor_rowstride * y;
		for (x = 0; x < icon->width; x++) {
			switch (icon->depth) {
			case 32:
				xor[0] = p[2];
				xor[1] = p[1];
				xor[2] = p[0];
				xor[3] = 0xff;
				if (n_channels == 4) {
					xor[3] = p[3];
					if (p[3] < 0x80)
						*and |= 1 << (7 - x % 8);
				}
				xor += 4;
				break;
			case 24:
				xor[0] = p[2];
				xor[1] = p[1];
				xor[2] = p[0];
				if (n_channels == 4 && p[3] < 0x80)
					*and |= 1 << (7 - x % 8);
				xor += 3;
				break;
			case 16:
				v = ((p[0] >> 3) << 10) | ((p[1] >> 3) << 5) | (p[2] >> 3);
				xor[0] = v & 0xff;
				xor[1] = v >> 8;
				if (n_channels == 4 && p[3] < 0x80)
					*and |= 1 << (7 - x % 8);
				xor += 2;
				break;
			}
			
			p += n_channels;
			if (x % 8 == 7) 
				and++;
		}
	}

	return TRUE;
}