create_generic_typespec (MonoDynamicImage *assembly, MonoReflectionTypeBuilder *tb)
{
	MonoDynamicTable *table;
	MonoClass *klass;
	MonoType *type;
	guint32 *values;
	guint32 token;
	SigBuffer buf;
	int count, i;

	/*
	 * We're creating a TypeSpec for the TypeBuilder of a generic type declaration,
	 * ie. what we'd normally use as the generic type in a TypeSpec signature.
	 * Because of this, we must not insert it into the `typeref' hash table.
	 */
	type = mono_reflection_type_get_handle ((MonoReflectionType*)tb);
	token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->typespec, type));
	if (token)
		return token;

	sigbuffer_init (&buf, 32);

	g_assert (tb->generic_params);
	klass = mono_class_from_mono_type (type);

	if (tb->generic_container)
		mono_reflection_create_generic_class (tb);

	sigbuffer_add_value (&buf, MONO_TYPE_GENERICINST);
	g_assert (klass->generic_container);
	sigbuffer_add_value (&buf, klass->byval_arg.type);
	sigbuffer_add_value (&buf, mono_image_typedef_or_ref_full (assembly, &klass->byval_arg, FALSE));

	count = mono_array_length (tb->generic_params);
	sigbuffer_add_value (&buf, count);
	for (i = 0; i < count; i++) {
		MonoReflectionGenericParam *gparam;

		gparam = mono_array_get (tb->generic_params, MonoReflectionGenericParam *, i);

		encode_type (assembly, mono_reflection_type_get_handle ((MonoReflectionType*)gparam), &buf);
	}

	table = &assembly->tables [MONO_TABLE_TYPESPEC];

	if (assembly->save) {
		token = sigbuffer_add_to_blob_cached (assembly, &buf);
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_TYPESPEC_SIZE;
		values [MONO_TYPESPEC_SIGNATURE] = token;
	}
	sigbuffer_free (&buf);

	token = MONO_TYPEDEFORREF_TYPESPEC | (table->next_idx << MONO_TYPEDEFORREF_BITS);
	g_hash_table_insert (assembly->typespec, type, GUINT_TO_POINTER(token));
	table->next_idx ++;
	return token;
}