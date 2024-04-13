mono_image_get_methodref_token (MonoDynamicImage *assembly, MonoMethod *method, gboolean create_typespec)
{
	guint32 token;
	MonoMethodSignature *sig;
	
	create_typespec = create_typespec && method->is_generic && method->klass->image != &assembly->image;

	if (create_typespec) {
		token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->handleref, GUINT_TO_POINTER (GPOINTER_TO_UINT (method) + 1)));
		if (token)
			return token;
	} 

	token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->handleref, method));
	if (token && !create_typespec)
		return token;

	g_assert (!method->is_inflated);
	if (!token) {
		/*
		 * A methodref signature can't contain an unmanaged calling convention.
		 */
		sig = mono_metadata_signature_dup (mono_method_signature (method));
		if ((sig->call_convention != MONO_CALL_DEFAULT) && (sig->call_convention != MONO_CALL_VARARG))
			sig->call_convention = MONO_CALL_DEFAULT;
		token = mono_image_get_memberref_token (assembly, &method->klass->byval_arg,
			method->name,  method_encode_signature (assembly, sig));
		g_free (sig);
		g_hash_table_insert (assembly->handleref, method, GUINT_TO_POINTER(token));
	}

	if (create_typespec) {
		MonoDynamicTable *table = &assembly->tables [MONO_TABLE_METHODSPEC];
		g_assert (mono_metadata_token_table (token) == MONO_TABLE_MEMBERREF);
		token = (mono_metadata_token_index (token) << MONO_METHODDEFORREF_BITS) | MONO_METHODDEFORREF_METHODREF;

		if (assembly->save) {
			guint32 *values;

			alloc_table (table, table->rows + 1);
			values = table->values + table->next_idx * MONO_METHODSPEC_SIZE;
			values [MONO_METHODSPEC_METHOD] = token;
			values [MONO_METHODSPEC_SIGNATURE] = encode_generic_method_sig (assembly, &mono_method_get_generic_container (method)->context);
		}

		token = MONO_TOKEN_METHOD_SPEC | table->next_idx;
		table->next_idx ++;
		/*methodspec and memberef tokens are diferent, */
		g_hash_table_insert (assembly->handleref, GUINT_TO_POINTER (GPOINTER_TO_UINT (method) + 1), GUINT_TO_POINTER (token));
		return token;
	}
	return token;
}