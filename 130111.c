mono_custom_attrs_has_attr (MonoCustomAttrInfo *ainfo, MonoClass *attr_klass)
{
	int i;
	MonoClass *klass;
	for (i = 0; i < ainfo->num_attrs; ++i) {
		klass = ainfo->attrs [i].ctor->klass;
		if (mono_class_has_parent (klass, attr_klass) || (MONO_CLASS_IS_INTERFACE (attr_klass) && mono_class_is_assignable_from (attr_klass, klass)))
			return TRUE;
	}
	return FALSE;
}