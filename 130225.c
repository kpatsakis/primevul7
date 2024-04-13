encode_constraints (MonoReflectionGenericParam *gparam, guint32 owner, MonoDynamicImage *assembly)
{
	MonoDynamicTable *table;
	guint32 num_constraints, i;
	guint32 *values;
	guint32 table_idx;

	table = &assembly->tables [MONO_TABLE_GENERICPARAMCONSTRAINT];
	num_constraints = gparam->iface_constraints ?
		mono_array_length (gparam->iface_constraints) : 0;
	table->rows += num_constraints;
	if (gparam->base_type)
		table->rows++;
	alloc_table (table, table->rows);

	if (gparam->base_type) {
		table_idx = table->next_idx ++;
		values = table->values + table_idx * MONO_GENPARCONSTRAINT_SIZE;

		values [MONO_GENPARCONSTRAINT_GENERICPAR] = owner;
		values [MONO_GENPARCONSTRAINT_CONSTRAINT] = mono_image_typedef_or_ref (
			assembly, mono_reflection_type_get_handle (gparam->base_type));
	}

	for (i = 0; i < num_constraints; i++) {
		MonoReflectionType *constraint = mono_array_get (
			gparam->iface_constraints, gpointer, i);

		table_idx = table->next_idx ++;
		values = table->values + table_idx * MONO_GENPARCONSTRAINT_SIZE;

		values [MONO_GENPARCONSTRAINT_GENERICPAR] = owner;
		values [MONO_GENPARCONSTRAINT_CONSTRAINT] = mono_image_typedef_or_ref (
			assembly, mono_reflection_type_get_handle (constraint));
	}
}