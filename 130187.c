mono_custom_attrs_from_index (MonoImage *image, guint32 idx)
{
	guint32 mtoken, i, len;
	guint32 cols [MONO_CUSTOM_ATTR_SIZE];
	MonoTableInfo *ca;
	MonoCustomAttrInfo *ainfo;
	GList *tmp, *list = NULL;
	const char *data;

	ca = &image->tables [MONO_TABLE_CUSTOMATTRIBUTE];

	i = mono_metadata_custom_attrs_from_index (image, idx);
	if (!i)
		return NULL;
	i --;
	while (i < ca->rows) {
		if (mono_metadata_decode_row_col (ca, i, MONO_CUSTOM_ATTR_PARENT) != idx)
			break;
		list = g_list_prepend (list, GUINT_TO_POINTER (i));
		++i;
	}
	len = g_list_length (list);
	if (!len)
		return NULL;
	ainfo = g_malloc0 (MONO_SIZEOF_CUSTOM_ATTR_INFO + sizeof (MonoCustomAttrEntry) * len);
	ainfo->num_attrs = len;
	ainfo->image = image;
	for (i = 0, tmp = list; i < len; ++i, tmp = tmp->next) {
		mono_metadata_decode_row (ca, GPOINTER_TO_UINT (tmp->data), cols, MONO_CUSTOM_ATTR_SIZE);
		mtoken = cols [MONO_CUSTOM_ATTR_TYPE] >> MONO_CUSTOM_ATTR_TYPE_BITS;
		switch (cols [MONO_CUSTOM_ATTR_TYPE] & MONO_CUSTOM_ATTR_TYPE_MASK) {
		case MONO_CUSTOM_ATTR_TYPE_METHODDEF:
			mtoken |= MONO_TOKEN_METHOD_DEF;
			break;
		case MONO_CUSTOM_ATTR_TYPE_MEMBERREF:
			mtoken |= MONO_TOKEN_MEMBER_REF;
			break;
		default:
			g_error ("Unknown table for custom attr type %08x", cols [MONO_CUSTOM_ATTR_TYPE]);
			break;
		}
		ainfo->attrs [i].ctor = mono_get_method (image, mtoken, NULL);
		if (!ainfo->attrs [i].ctor) {
			g_warning ("Can't find custom attr constructor image: %s mtoken: 0x%08x", image->name, mtoken);
			g_list_free (list);
			g_free (ainfo);
			return NULL;
		}

		if (!mono_verifier_verify_cattr_blob (image, cols [MONO_CUSTOM_ATTR_VALUE], NULL)) {
			/*FIXME raising an exception here doesn't make any sense*/
			g_warning ("Invalid custom attribute blob on image %s for index %x", image->name, idx);
			g_list_free (list);
			g_free (ainfo);
			return NULL;
		}
		data = mono_metadata_blob_heap (image, cols [MONO_CUSTOM_ATTR_VALUE]);
		ainfo->attrs [i].data_size = mono_metadata_decode_value (data, &data);
		ainfo->attrs [i].data = (guchar*)data;
	}
	g_list_free (list);

	return ainfo;
}