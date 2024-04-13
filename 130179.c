mono_custom_attrs_from_event (MonoClass *klass, MonoEvent *event)
{
	guint32 idx;
	
	if (klass->image->dynamic) {
		event = mono_metadata_get_corresponding_event_from_generic_type_definition (event);
		return lookup_custom_attr (klass->image, event);
	}
	idx = find_event_index (klass, event);
	idx <<= MONO_CUSTOM_ATTR_BITS;
	idx |= MONO_CUSTOM_ATTR_EVENT;
	return mono_custom_attrs_from_index (klass->image, idx);
}