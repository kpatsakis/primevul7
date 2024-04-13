mono_image_add_methodimpl (MonoDynamicImage *assembly, MonoReflectionMethodBuilder *mb)
{
	MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)mb->type;
	MonoDynamicTable *table;
	guint32 *values;
	guint32 tok;

	if (!mb->override_method)
		return;

	table = &assembly->tables [MONO_TABLE_METHODIMPL];
	table->rows ++;
	alloc_table (table, table->rows);
	values = table->values + table->rows * MONO_METHODIMPL_SIZE;
	values [MONO_METHODIMPL_CLASS] = tb->table_idx;
	values [MONO_METHODIMPL_BODY] = MONO_METHODDEFORREF_METHODDEF | (mb->table_idx << MONO_METHODDEFORREF_BITS);

	tok = mono_image_create_token (assembly, (MonoObject*)mb->override_method, FALSE, FALSE);
	switch (mono_metadata_token_table (tok)) {
	case MONO_TABLE_MEMBERREF:
		tok = (mono_metadata_token_index (tok) << MONO_METHODDEFORREF_BITS ) | MONO_METHODDEFORREF_METHODREF;
		break;
	case MONO_TABLE_METHOD:
		tok = (mono_metadata_token_index (tok) << MONO_METHODDEFORREF_BITS ) | MONO_METHODDEFORREF_METHODDEF;
		break;
	default:
		g_assert_not_reached ();
	}
	values [MONO_METHODIMPL_DECLARATION] = tok;
}