mono_custom_attrs_from_class (MonoClass *klass)
{
	guint32 idx;

	if (klass->generic_class)
		klass = klass->generic_class->container_class;

	if (klass->image->dynamic)
		return lookup_custom_attr (klass->image, klass);

	if (klass->byval_arg.type == MONO_TYPE_VAR || klass->byval_arg.type == MONO_TYPE_MVAR) {
		idx = mono_metadata_token_index (klass->sizes.generic_param_token);
		idx <<= MONO_CUSTOM_ATTR_BITS;
		idx |= MONO_CUSTOM_ATTR_GENERICPAR;
	} else {
		idx = mono_metadata_token_index (klass->type_token);
		idx <<= MONO_CUSTOM_ATTR_BITS;
		idx |= MONO_CUSTOM_ATTR_TYPEDEF;
	}
	return mono_custom_attrs_from_index (klass->image, idx);
}