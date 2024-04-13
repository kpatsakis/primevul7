_g_settings_new_if_schema_installed (const char *schema_id)
{
	GSettingsSchema *schema;

	schema = g_settings_schema_source_lookup (g_settings_schema_source_get_default (),
						  schema_id,
						  TRUE);
	if (schema == NULL)
		return NULL;

	g_settings_schema_unref (schema);

	return g_settings_new (schema_id);
}