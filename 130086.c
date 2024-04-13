mono_image_get_method_info (MonoReflectionMethodBuilder *mb, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 *values;
	ReflectionMethodBuilder rmb;
	int i;

	reflection_methodbuilder_from_method_builder (&rmb, mb);

	mono_image_basic_method (&rmb, assembly);
	mb->table_idx = *rmb.table_idx;

	if (mb->dll) { /* It's a P/Invoke method */
		guint32 moduleref;
		/* map CharSet values to on-disk values */
		int ncharset = (mb->charset ? (mb->charset - 1) * 2 : 0);
		int extra_flags = mb->extra_flags;
		table = &assembly->tables [MONO_TABLE_IMPLMAP];
		table->rows ++;
		alloc_table (table, table->rows);
		values = table->values + table->rows * MONO_IMPLMAP_SIZE;
		
		values [MONO_IMPLMAP_FLAGS] = (mb->native_cc << 8) | ncharset | extra_flags;
		values [MONO_IMPLMAP_MEMBER] = (mb->table_idx << 1) | 1; /* memberforwarded: method */
		if (mb->dllentry)
			values [MONO_IMPLMAP_NAME] = string_heap_insert_mstring (&assembly->sheap, mb->dllentry);
		else
			values [MONO_IMPLMAP_NAME] = string_heap_insert_mstring (&assembly->sheap, mb->name);
		moduleref = string_heap_insert_mstring (&assembly->sheap, mb->dll);
		if (!(values [MONO_IMPLMAP_SCOPE] = find_index_in_table (assembly, MONO_TABLE_MODULEREF, MONO_MODULEREF_NAME, moduleref))) {
			table = &assembly->tables [MONO_TABLE_MODULEREF];
			table->rows ++;
			alloc_table (table, table->rows);
			table->values [table->rows * MONO_MODULEREF_SIZE + MONO_MODULEREF_NAME] = moduleref;
			values [MONO_IMPLMAP_SCOPE] = table->rows;
		}
	}

	if (mb->generic_params) {
		table = &assembly->tables [MONO_TABLE_GENERICPARAM];
		table->rows += mono_array_length (mb->generic_params);
		alloc_table (table, table->rows);
		for (i = 0; i < mono_array_length (mb->generic_params); ++i) {
			guint32 owner = MONO_TYPEORMETHOD_METHOD | (mb->table_idx << MONO_TYPEORMETHOD_BITS);

			mono_image_get_generic_param_info (
				mono_array_get (mb->generic_params, gpointer, i), owner, assembly);
		}
	}

}