string_heap_insert_mstring (MonoDynamicStream *sh, MonoString *str)
{
	char *name = mono_string_to_utf8 (str);
	guint32 idx;
	idx = string_heap_insert (sh, name);
	g_free (name);
	return idx;
}