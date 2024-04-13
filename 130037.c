mono_image_fill_file_table (MonoDomain *domain, MonoReflectionModule *module, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 *values;
	char blob_size [6];
	guchar hash [20];
	char *b = blob_size;
	char *dir, *path;

	table = &assembly->tables [MONO_TABLE_FILE];
	table->rows++;
	alloc_table (table, table->rows);
	values = table->values + table->next_idx * MONO_FILE_SIZE;
	values [MONO_FILE_FLAGS] = FILE_CONTAINS_METADATA;
	values [MONO_FILE_NAME] = string_heap_insert (&assembly->sheap, module->image->module_name);
	if (module->image->dynamic) {
		/* This depends on the fact that the main module is emitted last */
		dir = mono_string_to_utf8 (((MonoReflectionModuleBuilder*)module)->assemblyb->dir);
		path = g_strdup_printf ("%s%c%s", dir, G_DIR_SEPARATOR, module->image->module_name);
	} else {
		dir = NULL;
		path = g_strdup (module->image->name);
	}
	mono_sha1_get_digest_from_file (path, hash);
	g_free (dir);
	g_free (path);
	mono_metadata_encode_value (20, b, &b);
	values [MONO_FILE_HASH_VALUE] = mono_image_add_stream_data (&assembly->blob, blob_size, b-blob_size);
	mono_image_add_stream_data (&assembly->blob, (char*)hash, 20);
	table->next_idx ++;
}