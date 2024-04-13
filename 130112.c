mono_image_typedef_or_ref_full (MonoDynamicImage *assembly, MonoType *type, gboolean try_typespec)
{
	MonoDynamicTable *table;
	guint32 *values;
	guint32 token, scope, enclosing;
	MonoClass *klass;

	/* if the type requires a typespec, we must try that first*/
	if (try_typespec && (token = create_typespec (assembly, type)))
		return token;
	token = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->typeref, type));
	if (token)
		return token;
	klass = mono_class_from_mono_type (type);
	if (!klass)
		klass = mono_class_from_mono_type (type);

	/*
	 * If it's in the same module and not a generic type parameter:
	 */
	if ((klass->image == &assembly->image) && (type->type != MONO_TYPE_VAR) && 
			(type->type != MONO_TYPE_MVAR)) {
		MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);
		token = MONO_TYPEDEFORREF_TYPEDEF | (tb->table_idx << MONO_TYPEDEFORREF_BITS);
		mono_g_hash_table_insert (assembly->tokens, GUINT_TO_POINTER (token), mono_class_get_ref_info (klass));
		return token;
	}

	if (klass->nested_in) {
		enclosing = mono_image_typedef_or_ref_full (assembly, &klass->nested_in->byval_arg, FALSE);
		/* get the typeref idx of the enclosing type */
		enclosing >>= MONO_TYPEDEFORREF_BITS;
		scope = (enclosing << MONO_RESOLTION_SCOPE_BITS) | MONO_RESOLTION_SCOPE_TYPEREF;
	} else {
		scope = resolution_scope_from_image (assembly, klass->image);
	}
	table = &assembly->tables [MONO_TABLE_TYPEREF];
	if (assembly->save) {
		alloc_table (table, table->rows + 1);
		values = table->values + table->next_idx * MONO_TYPEREF_SIZE;
		values [MONO_TYPEREF_SCOPE] = scope;
		values [MONO_TYPEREF_NAME] = string_heap_insert (&assembly->sheap, klass->name);
		values [MONO_TYPEREF_NAMESPACE] = string_heap_insert (&assembly->sheap, klass->name_space);
	}
	token = MONO_TYPEDEFORREF_TYPEREF | (table->next_idx << MONO_TYPEDEFORREF_BITS); /* typeref */
	g_hash_table_insert (assembly->typeref, type, GUINT_TO_POINTER(token));
	table->next_idx ++;
	mono_g_hash_table_insert (assembly->tokens, GUINT_TO_POINTER (token), mono_class_get_ref_info (klass));
	return token;
}