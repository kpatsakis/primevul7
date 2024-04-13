encode_reflection_type (MonoDynamicImage *assembly, MonoReflectionType *type, SigBuffer *buf)
{
	if (!type) {
		sigbuffer_add_value (buf, MONO_TYPE_VOID);
		return;
	}

	encode_type (assembly, mono_reflection_type_get_handle (type), buf);
}