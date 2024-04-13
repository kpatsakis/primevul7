void
mono_reflection_create_internal_class (MonoReflectionTypeBuilder *tb)
{
	MonoClass *klass;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (tb->type.type);

	mono_loader_lock ();
	if (klass->enumtype && mono_class_enum_basetype (klass) == NULL) {
		MonoReflectionFieldBuilder *fb;
		MonoClass *ec;
		MonoType *enum_basetype;

		g_assert (tb->fields != NULL);
		g_assert (mono_array_length (tb->fields) >= 1);

		fb = mono_array_get (tb->fields, MonoReflectionFieldBuilder*, 0);

		if (!mono_type_is_valid_enum_basetype (mono_reflection_type_get_handle ((MonoReflectionType*)fb->type))) {
			mono_loader_unlock ();
			return;
		}

		enum_basetype = mono_reflection_type_get_handle ((MonoReflectionType*)fb->type);
		klass->element_class = mono_class_from_mono_type (enum_basetype);
		if (!klass->element_class)
			klass->element_class = mono_class_from_mono_type (enum_basetype);

		/*
		 * get the element_class from the current corlib.
		 */
		ec = default_class_from_mono_type (enum_basetype);
		klass->instance_size = ec->instance_size;
		klass->size_inited = 1;
		/* 
		 * this is almost safe to do with enums and it's needed to be able
		 * to create objects of the enum type (for use in SetConstant).
		 */
		/* FIXME: Does this mean enums can't have method overrides ? */
		mono_class_setup_vtable_general (klass, NULL, 0);
	}
	mono_loader_unlock ();