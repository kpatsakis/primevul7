mono_image_get_generic_param_info (MonoReflectionGenericParam *gparam, guint32 owner, MonoDynamicImage *assembly)
{
	GenericParamTableEntry *entry;

	/*
	 * The GenericParam table must be sorted according to the `owner' field.
	 * We need to do this sorting prior to writing the GenericParamConstraint
	 * table, since we have to use the final GenericParam table indices there
	 * and they must also be sorted.
	 */

	entry = g_new0 (GenericParamTableEntry, 1);
	entry->owner = owner;
	/* FIXME: track where gen_params should be freed and remove the GC root as well */
	MOVING_GC_REGISTER (&entry->gparam);
	entry->gparam = gparam;
	
	g_ptr_array_add (assembly->gen_params, entry);
}