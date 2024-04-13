static void
ensure_complete_type (MonoClass *klass)
{
	if (klass->image->dynamic && !klass->wastypebuilder && mono_class_get_ref_info (klass)) {
		MonoReflectionTypeBuilder *tb = mono_class_get_ref_info (klass);

		mono_domain_try_type_resolve (mono_domain_get (), NULL, (MonoObject*)tb);

		// Asserting here could break a lot of code
		//g_assert (klass->wastypebuilder);
	}

	if (klass->generic_class) {
		MonoGenericInst *inst = klass->generic_class->context.class_inst;
		int i;

		for (i = 0; i < inst->type_argc; ++i) {
			ensure_complete_type (mono_class_from_mono_type (inst->type_argv [i]));
		}
	}