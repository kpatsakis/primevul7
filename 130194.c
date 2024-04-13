mono_image_typedef_or_ref (MonoDynamicImage *assembly, MonoType *type)
{
	return mono_image_typedef_or_ref_full (assembly, type, TRUE);
}