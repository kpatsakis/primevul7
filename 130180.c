collect_types (MonoPtrArray *types, MonoReflectionTypeBuilder *type)
{
	int i;

	mono_ptr_array_append (*types, type);

	if (!type->subtypes)
		return;

	for (i = 0; i < mono_array_length (type->subtypes); ++i) {
		MonoReflectionTypeBuilder *subtype = mono_array_get (type->subtypes, MonoReflectionTypeBuilder*, i);
		collect_types (types, subtype);
	}
}