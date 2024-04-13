resource_tree_encode (ResTreeNode *node, char *begin, char *p, char **endbuf)
{
	char *entries;
	MonoPEResourceDir dir;
	MonoPEResourceDirEntry dir_entry;
	MonoPEResourceDataEntry data_entry;
	GSList *l;
	guint32 res_id_entries;

	/*
	 * For the format of the resource directory, see the article
	 * "An In-Depth Look into the Win32 Portable Executable File Format" by
	 * Matt Pietrek
	 */

	memset (&dir, 0, sizeof (dir));
	memset (&dir_entry, 0, sizeof (dir_entry));
	memset (&data_entry, 0, sizeof (data_entry));

	g_assert (sizeof (dir) == 16);
	g_assert (sizeof (dir_entry) == 8);
	g_assert (sizeof (data_entry) == 16);

	node->offset = p - begin;

	/* IMAGE_RESOURCE_DIRECTORY */
	res_id_entries = g_slist_length (node->children);
	dir.res_id_entries = GUINT16_TO_LE (res_id_entries);

	memcpy (p, &dir, sizeof (dir));
	p += sizeof (dir);

	/* Reserve space for entries */
	entries = p;
	p += sizeof (dir_entry) * res_id_entries;

	/* Write children */
	for (l = node->children; l; l = l->next) {
		ResTreeNode *child = (ResTreeNode*)l->data;

		if (child->win32_res) {
			guint32 size;

			child->offset = p - begin;

			/* IMAGE_RESOURCE_DATA_ENTRY */
			data_entry.rde_data_offset = GUINT32_TO_LE (p - begin + sizeof (data_entry));
			size = mono_array_length (child->win32_res->res_data);
			data_entry.rde_size = GUINT32_TO_LE (size);

			memcpy (p, &data_entry, sizeof (data_entry));
			p += sizeof (data_entry);

			memcpy (p, mono_array_addr (child->win32_res->res_data, char, 0), size);
			p += size;
		} else {
			resource_tree_encode (child, begin, p, &p);
		}
	}

	/* IMAGE_RESOURCE_ENTRY */
	for (l = node->children; l; l = l->next) {
		ResTreeNode *child = (ResTreeNode*)l->data;

		MONO_PE_RES_DIR_ENTRY_SET_NAME (dir_entry, FALSE, child->id);
		MONO_PE_RES_DIR_ENTRY_SET_DIR (dir_entry, !child->win32_res, child->offset);

		memcpy (entries, &dir_entry, sizeof (dir_entry));
		entries += sizeof (dir_entry);
	}

	*endbuf = p;
}