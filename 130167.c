mono_image_basic_method (ReflectionMethodBuilder *mb, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint i, count;

	/* room in this table is already allocated */
	table = &assembly->tables [MONO_TABLE_METHOD];
	*mb->table_idx = table->next_idx ++;
	g_hash_table_insert (assembly->method_to_table_idx, mb->mhandle, GUINT_TO_POINTER ((*mb->table_idx)));
	values = table->values + *mb->table_idx * MONO_METHOD_SIZE;
	values [MONO_METHOD_NAME] = string_heap_insert_mstring (&assembly->sheap, mb->name);
	values [MONO_METHOD_FLAGS] = mb->attrs;
	values [MONO_METHOD_IMPLFLAGS] = mb->iattrs;
	values [MONO_METHOD_SIGNATURE] = method_builder_encode_signature (assembly, mb);
	values [MONO_METHOD_RVA] = method_encode_code (assembly, mb);
	
	table = &assembly->tables [MONO_TABLE_PARAM];
	values [MONO_METHOD_PARAMLIST] = table->next_idx;

	mono_image_add_decl_security (assembly, 
		mono_metadata_make_token (MONO_TABLE_METHOD, *mb->table_idx), mb->permissions);

	if (mb->pinfo) {
		MonoDynamicTable *mtable;
		guint32 *mvalues;
		
		mtable = &assembly->tables [MONO_TABLE_FIELDMARSHAL];
		mvalues = mtable->values + mtable->next_idx * MONO_FIELD_MARSHAL_SIZE;
		
		count = 0;
		for (i = 0; i < mono_array_length (mb->pinfo); ++i) {
			if (mono_array_get (mb->pinfo, gpointer, i))
				count++;
		}
		table->rows += count;
		alloc_table (table, table->rows);
		values = table->values + table->next_idx * MONO_PARAM_SIZE;
		for (i = 0; i < mono_array_length (mb->pinfo); ++i) {
			MonoReflectionParamBuilder *pb;
			if ((pb = mono_array_get (mb->pinfo, MonoReflectionParamBuilder*, i))) {
				values [MONO_PARAM_FLAGS] = pb->attrs;
				values [MONO_PARAM_SEQUENCE] = i;
				if (pb->name != NULL) {
					values [MONO_PARAM_NAME] = string_heap_insert_mstring (&assembly->sheap, pb->name);
				} else {
					values [MONO_PARAM_NAME] = 0;
				}
				values += MONO_PARAM_SIZE;
				if (pb->marshal_info) {
					mtable->rows++;
					alloc_table (mtable, mtable->rows);
					mvalues = mtable->values + mtable->rows * MONO_FIELD_MARSHAL_SIZE;
					mvalues [MONO_FIELD_MARSHAL_PARENT] = (table->next_idx << MONO_HAS_FIELD_MARSHAL_BITS) | MONO_HAS_FIELD_MARSHAL_PARAMDEF;
					mvalues [MONO_FIELD_MARSHAL_NATIVE_TYPE] = encode_marshal_blob (assembly, pb->marshal_info);
				}
				pb->table_idx = table->next_idx++;
				if (pb->attrs & PARAM_ATTRIBUTE_HAS_DEFAULT) {
					guint32 field_type = 0;
					mtable = &assembly->tables [MONO_TABLE_CONSTANT];
					mtable->rows ++;
					alloc_table (mtable, mtable->rows);
					mvalues = mtable->values + mtable->rows * MONO_CONSTANT_SIZE;
					mvalues [MONO_CONSTANT_PARENT] = MONO_HASCONSTANT_PARAM | (pb->table_idx << MONO_HASCONSTANT_BITS);
					mvalues [MONO_CONSTANT_VALUE] = encode_constant (assembly, pb->def_value, &field_type);
					mvalues [MONO_CONSTANT_TYPE] = field_type;
					mvalues [MONO_CONSTANT_PADDING] = 0;
				}
			}
		}
	}
}