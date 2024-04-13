mono_custom_attrs_get_attr (MonoCustomAttrInfo *ainfo, MonoClass *attr_klass)
{
	int i, attr_index;
	MonoClass *klass;
	MonoArray *attrs;

	attr_index = -1;
	for (i = 0; i < ainfo->num_attrs; ++i) {
		klass = ainfo->attrs [i].ctor->klass;
		if (mono_class_has_parent (klass, attr_klass)) {
			attr_index = i;
			break;
		}
	}
	if (attr_index == -1)
		return NULL;

	attrs = mono_custom_attrs_construct (ainfo);
	if (attrs)
		return mono_array_get (attrs, MonoObject*, attr_index);
	else
		return NULL;
}