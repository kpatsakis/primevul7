write_icon (FILE *f, GSList *entries)
{
	IconEntry *icon;
	GSList *entry;
	guint8 bytes[4];
	guint16 words[4];
	guint32 dwords[6];
	gint type;
	gint n_entries;
	gint offset;
	gint size;

	if (((IconEntry *)entries->data)->hot_x > -1)
		type = 2;
	else 
		type = 1;
	n_entries = g_slist_length (entries);

	/* header */
	words[0] = 0;
	words[1] = type;
	words[2] = n_entries;
	write16 (f, words, 3);
	
	offset = 6 + 16 * n_entries;

	for (entry = entries; entry; entry = entry->next) {
		icon = (IconEntry *)entry->data;
		size = 40 + icon->height * (icon->and_rowstride + icon->xor_rowstride);
		
		/* directory entry */
		if (icon->width == 256)
			bytes[0] = 0;
		else
			bytes[0] = icon->width;
		if (icon->height == 256)
			bytes[1] = 0;
		else
			bytes[1] = icon->height;
		bytes[2] = icon->n_colors;
		bytes[3] = 0;
		write8 (f, bytes, 4);
		if (type == 1) {
			words[0] = 1;
			words[1] = icon->depth;
		}
		else {
			words[0] = icon->hot_x;
			words[1] = icon->hot_y;
		}
		write16 (f, words, 2);
		dwords[0] = size;
		dwords[1] = offset;
		write32 (f, dwords, 2);

		offset += size;
	}

	for (entry = entries; entry; entry = entry->next) {
		icon = (IconEntry *)entry->data;

		/* bitmap header */
		dwords[0] = 40;
		dwords[1] = icon->width;
		dwords[2] = icon->height * 2;
		write32 (f, dwords, 3);
		words[0] = 1;
		words[1] = icon->depth;
		write16 (f, words, 2);
		dwords[0] = 0;
		dwords[1] = 0;
		dwords[2] = 0;
		dwords[3] = 0;
		dwords[4] = 0;
		dwords[5] = 0;
		write32 (f, dwords, 6);

		/* image data */
		write8 (f, icon->xor, icon->xor_rowstride * icon->height);
		write8 (f, icon->and, icon->and_rowstride * icon->height);
	}
}