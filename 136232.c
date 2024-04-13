gdk_pixbuf__ico_image_save (FILE          *f, 
                            GdkPixbuf     *pixbuf, 
                            gchar        **keys,
                            gchar        **values,
                            GError       **error)
{
	gint hot_x, hot_y;
	IconEntry *icon;
	GSList *entries = NULL;

	/* support only single-image ICOs for now */
	icon = g_new0 (IconEntry, 1);
	icon->width = gdk_pixbuf_get_width (pixbuf);
	icon->height = gdk_pixbuf_get_height (pixbuf);
	icon->depth = gdk_pixbuf_get_has_alpha (pixbuf) ? 32 : 24;
	hot_x = -1;
	hot_y = -1;
	
	/* parse options */
	if (keys && *keys) {
		gchar **kiter;
		gchar **viter;
		
		for (kiter = keys, viter = values; *kiter && *viter; kiter++, viter++) {
			char *endptr;
			if (strcmp (*kiter, "depth") == 0) {
				sscanf (*viter, "%d", &icon->depth);
			}
			else if (strcmp (*kiter, "x_hot") == 0) {
				hot_x = strtol (*viter, &endptr, 10);
			}
			else if (strcmp (*kiter, "y_hot") == 0) {
				hot_y = strtol (*viter, &endptr, 10);
			}

		}
	}

	if (!fill_entry (icon, pixbuf, hot_x, hot_y, error)) {
		free_entry (icon);
		return FALSE;
	}

	entries = g_slist_append (entries, icon); 
	write_icon (f, entries);

	g_slist_foreach (entries, (GFunc)free_entry, NULL);
	g_slist_free (entries);

	return TRUE;
}