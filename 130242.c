mono_reflection_get_type (MonoImage* image, MonoTypeNameParse *info, gboolean ignorecase, gboolean *type_resolve) {
	return mono_reflection_get_type_with_rootimage(image, image, info, ignorecase, type_resolve);
}