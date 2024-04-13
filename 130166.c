mono_save_custom_attrs (MonoImage *image, void *obj, MonoArray *cattrs)
{
	MonoCustomAttrInfo *ainfo, *tmp;

	if (!cattrs || !mono_array_length (cattrs))
		return;

	ainfo = mono_custom_attrs_from_builders (image, image, cattrs);

	mono_loader_lock ();
	tmp = mono_image_property_lookup (image, obj, MONO_PROP_DYNAMIC_CATTR);
	if (tmp)
		mono_custom_attrs_free (tmp);
	mono_image_property_insert (image, obj, MONO_PROP_DYNAMIC_CATTR, ainfo);
	mono_loader_unlock ();

}