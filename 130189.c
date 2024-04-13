mono_image_fill_export_table_from_class (MonoDomain *domain, MonoClass *klass,
	guint32 module_index, guint32 parent_index, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 visib, res;

	visib = klass->flags & TYPE_ATTRIBUTE_VISIBILITY_MASK;
	if (! ((visib & TYPE_ATTRIBUTE_PUBLIC) || (visib & TYPE_ATTRIBUTE_NESTED_PUBLIC)))
		return 0;

	table = &assembly->tables [MONO_TABLE_EXPORTEDTYPE];
	table->rows++;
	alloc_table (table, table->rows);
	values = table->values + table->next_idx * MONO_EXP_TYPE_SIZE;

	values [MONO_EXP_TYPE_FLAGS] = klass->flags;
	values [MONO_EXP_TYPE_TYPEDEF] = klass->type_token;
	if (klass->nested_in)
		values [MONO_EXP_TYPE_IMPLEMENTATION] = (parent_index << MONO_IMPLEMENTATION_BITS) + MONO_IMPLEMENTATION_EXP_TYPE;
	else
		values [MONO_EXP_TYPE_IMPLEMENTATION] = (module_index << MONO_IMPLEMENTATION_BITS) + MONO_IMPLEMENTATION_FILE;
	values [MONO_EXP_TYPE_NAME] = string_heap_insert (&assembly->sheap, klass->name);
	values [MONO_EXP_TYPE_NAMESPACE] = string_heap_insert (&assembly->sheap, klass->name_space);

	res = table->next_idx;

	table->next_idx ++;

	/* Emit nested types */
	if (klass->ext && klass->ext->nested_classes) {
		GList *tmp;

		for (tmp = klass->ext->nested_classes; tmp; tmp = tmp->next)
			mono_image_fill_export_table_from_class (domain, tmp->data, module_index, table->next_idx - 1, assembly);
	}

	return res;
}