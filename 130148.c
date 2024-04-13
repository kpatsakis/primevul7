static guint
field_hash (gconstpointer key)
{
	const MonoClassField *field = key;
	return g_str_hash (field->name) ^ mono_metadata_type_hash (field->type); /**/