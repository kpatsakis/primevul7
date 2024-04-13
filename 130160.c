encode_marshal_blob (MonoDynamicImage *assembly, MonoReflectionMarshal *minfo) {
	char *str;
	SigBuffer buf;
	guint32 idx, len;

	sigbuffer_init (&buf, 32);

	sigbuffer_add_value (&buf, minfo->type);

	switch (minfo->type) {
	case MONO_NATIVE_BYVALTSTR:
	case MONO_NATIVE_BYVALARRAY:
		sigbuffer_add_value (&buf, minfo->count);
		break;
	case MONO_NATIVE_LPARRAY:
		if (minfo->eltype || minfo->has_size) {
			sigbuffer_add_value (&buf, minfo->eltype);
			if (minfo->has_size) {
				sigbuffer_add_value (&buf, minfo->param_num != -1? minfo->param_num: 0);
				sigbuffer_add_value (&buf, minfo->count != -1? minfo->count: 0);

				/* LAMESPEC: ElemMult is undocumented */
				sigbuffer_add_value (&buf, minfo->param_num != -1? 1: 0);
			}
		}
		break;
	case MONO_NATIVE_SAFEARRAY:
		if (minfo->eltype)
			sigbuffer_add_value (&buf, minfo->eltype);
		break;
	case MONO_NATIVE_CUSTOM:
		if (minfo->guid) {
			str = mono_string_to_utf8 (minfo->guid);
			len = strlen (str);
			sigbuffer_add_value (&buf, len);
			sigbuffer_add_mem (&buf, str, len);
			g_free (str);
		} else {
			sigbuffer_add_value (&buf, 0);
		}
		/* native type name */
		sigbuffer_add_value (&buf, 0);
		/* custom marshaler type name */
		if (minfo->marshaltype || minfo->marshaltyperef) {
			if (minfo->marshaltyperef)
				str = type_get_fully_qualified_name (mono_reflection_type_get_handle ((MonoReflectionType*)minfo->marshaltyperef));
			else
				str = mono_string_to_utf8 (minfo->marshaltype);
			len = strlen (str);
			sigbuffer_add_value (&buf, len);
			sigbuffer_add_mem (&buf, str, len);
			g_free (str);
		} else {
			/* FIXME: Actually a bug, since this field is required.  Punting for now ... */
			sigbuffer_add_value (&buf, 0);
		}
		if (minfo->mcookie) {
			str = mono_string_to_utf8 (minfo->mcookie);
			len = strlen (str);
			sigbuffer_add_value (&buf, len);
			sigbuffer_add_mem (&buf, str, len);
			g_free (str);
		} else {
			sigbuffer_add_value (&buf, 0);
		}
		break;
	default:
		break;
	}
	idx = sigbuffer_add_to_blob_cached (assembly, &buf);
	sigbuffer_free (&buf);
	return idx;
}