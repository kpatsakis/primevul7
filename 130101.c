mono_custom_attrs_construct (MonoCustomAttrInfo *cinfo)
{
	MonoArray *result;
	MonoObject *attr;
	int i;

	result = mono_array_new_cached (mono_domain_get (), mono_defaults.attribute_class, cinfo->num_attrs);
	for (i = 0; i < cinfo->num_attrs; ++i) {
		if (!cinfo->attrs [i].ctor)
			/* The cattr type is not finished yet */
			/* We should include the type name but cinfo doesn't contain it */
			mono_raise_exception (mono_get_exception_type_load (NULL, NULL));
		attr = create_custom_attr (cinfo->image, cinfo->attrs [i].ctor, cinfo->attrs [i].data, cinfo->attrs [i].data_size);
		mono_array_setref (result, i, attr);
	}
	return result;
}