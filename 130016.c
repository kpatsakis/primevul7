write_generic_param_entry (MonoDynamicImage *assembly, GenericParamTableEntry *entry)
{
	MonoDynamicTable *table;
	MonoGenericParam *param;
	guint32 *values;
	guint32 table_idx;

	table = &assembly->tables [MONO_TABLE_GENERICPARAM];
	table_idx = table->next_idx ++;
	values = table->values + table_idx * MONO_GENERICPARAM_SIZE;

	param = mono_reflection_type_get_handle ((MonoReflectionType*)entry->gparam)->data.generic_param;

	values [MONO_GENERICPARAM_OWNER] = entry->owner;
	values [MONO_GENERICPARAM_FLAGS] = entry->gparam->attrs;
	values [MONO_GENERICPARAM_NUMBER] = mono_generic_param_num (param);
	values [MONO_GENERICPARAM_NAME] = string_heap_insert (&assembly->sheap, mono_generic_param_info (param)->name);

	mono_image_add_cattrs (assembly, table_idx, MONO_CUSTOM_ATTR_GENERICPAR, entry->gparam->cattrs);

	encode_constraints (entry->gparam, table_idx, assembly);
}