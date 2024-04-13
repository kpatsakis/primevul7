MonoReflectionType*
mono_reflection_create_runtime_class (MonoReflectionTypeBuilder *tb)
{
	MonoError error;
	MonoClass *klass;
	MonoDomain* domain;
	MonoReflectionType* res;
	int i, j;

	MONO_ARCH_SAVE_REGS;

	domain = mono_object_domain (tb);
	klass = mono_class_from_mono_type (tb->type.type);

	/*
	 * Check for user defined Type subclasses.
	 */
	RESOLVE_TYPE (tb->parent);
	check_array_for_usertypes (tb->interfaces);
	if (tb->fields) {
		for (i = 0; i < mono_array_length (tb->fields); ++i) {
			MonoReflectionFieldBuilder *fb = mono_array_get (tb->fields, gpointer, i);
			if (fb) {
				RESOLVE_TYPE (fb->type);
				check_array_for_usertypes (fb->modreq);
				check_array_for_usertypes (fb->modopt);
				if (fb->marshal_info && fb->marshal_info->marshaltyperef)
					RESOLVE_TYPE (fb->marshal_info->marshaltyperef);
			}
		}
	}
	if (tb->methods) {
		for (i = 0; i < mono_array_length (tb->methods); ++i) {
			MonoReflectionMethodBuilder *mb = mono_array_get (tb->methods, gpointer, i);
			if (mb) {
				RESOLVE_TYPE (mb->rtype);
				check_array_for_usertypes (mb->return_modreq);
				check_array_for_usertypes (mb->return_modopt);
				check_array_for_usertypes (mb->parameters);
				if (mb->param_modreq)
					for (j = 0; j < mono_array_length (mb->param_modreq); ++j)
						check_array_for_usertypes (mono_array_get (mb->param_modreq, MonoArray*, j));
				if (mb->param_modopt)
					for (j = 0; j < mono_array_length (mb->param_modopt); ++j)
						check_array_for_usertypes (mono_array_get (mb->param_modopt, MonoArray*, j));
			}
		}
	}
	if (tb->ctors) {
		for (i = 0; i < mono_array_length (tb->ctors); ++i) {
			MonoReflectionCtorBuilder *mb = mono_array_get (tb->ctors, gpointer, i);
			if (mb) {
				check_array_for_usertypes (mb->parameters);
				if (mb->param_modreq)
					for (j = 0; j < mono_array_length (mb->param_modreq); ++j)
						check_array_for_usertypes (mono_array_get (mb->param_modreq, MonoArray*, j));
				if (mb->param_modopt)
					for (j = 0; j < mono_array_length (mb->param_modopt); ++j)
						check_array_for_usertypes (mono_array_get (mb->param_modopt, MonoArray*, j));
			}
		}
	}

	mono_save_custom_attrs (klass->image, klass, tb->cattrs);

	/* 
	 * we need to lock the domain because the lock will be taken inside
	 * So, we need to keep the locking order correct.
	 */
	mono_loader_lock ();
	mono_domain_lock (domain);
	if (klass->wastypebuilder) {
		mono_domain_unlock (domain);
		mono_loader_unlock ();
		return mono_type_get_object (mono_object_domain (tb), &klass->byval_arg);
	}
	/*
	 * Fields to set in klass:
	 * the various flags: delegate/unicode/contextbound etc.
	 */
	klass->flags = tb->attrs;
	klass->has_cctor = 1;
	klass->has_finalize = 1;

	/* fool mono_class_setup_parent */
	klass->supertypes = NULL;
	mono_class_setup_parent (klass, klass->parent);
	mono_class_setup_mono_type (klass);

#if 0
	if (!((MonoDynamicImage*)klass->image)->run) {
		if (klass->generic_container) {
			/* FIXME: The code below can't handle generic classes */
			klass->wastypebuilder = TRUE;
			mono_loader_unlock ();
			mono_domain_unlock (domain);
			return mono_type_get_object (mono_object_domain (tb), &klass->byval_arg);
		}
	}
#endif

	/* enums are done right away */
	if (!klass->enumtype)
		ensure_runtime_vtable (klass);

	if (tb->subtypes) {
		for (i = 0; i < mono_array_length (tb->subtypes); ++i) {
			MonoReflectionTypeBuilder *subtb = mono_array_get (tb->subtypes, MonoReflectionTypeBuilder*, i);
			mono_class_alloc_ext (klass);
			klass->ext->nested_classes = g_list_prepend_image (klass->image, klass->ext->nested_classes, mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)subtb)));
		}
	}

	klass->nested_classes_inited = TRUE;

	/* fields and object layout */
	if (klass->parent) {
		if (!klass->parent->size_inited)
			mono_class_init (klass->parent);
		klass->instance_size = klass->parent->instance_size;
		klass->sizes.class_size = 0;
		klass->min_align = klass->parent->min_align;
		/* if the type has no fields we won't call the field_setup
		 * routine which sets up klass->has_references.
		 */
		klass->has_references |= klass->parent->has_references;
	} else {
		klass->instance_size = sizeof (MonoObject);
		klass->min_align = 1;
	}

	/* FIXME: handle packing_size and instance_size */
	typebuilder_setup_fields (klass, &error);
	if (!mono_error_ok (&error))
		goto failure;
	typebuilder_setup_properties (klass, &error);
	if (!mono_error_ok (&error))
		goto failure;

	typebuilder_setup_events (klass, &error);
	if (!mono_error_ok (&error))
		goto failure;

	klass->wastypebuilder = TRUE;

	/* 
	 * If we are a generic TypeBuilder, there might be instantiations in the type cache
	 * which have type System.Reflection.MonoGenericClass, but after the type is created, 
	 * we want to return normal System.MonoType objects, so clear these out from the cache.
	 *
	 * Together with this we must ensure the contents of all instances to match the created type.
	 */
	if (domain->type_hash && klass->generic_container)
		mono_g_hash_table_foreach_remove (domain->type_hash, remove_instantiations_of_and_ensure_contents, klass);

	mono_domain_unlock (domain);
	mono_loader_unlock ();

	if (klass->enumtype && !mono_class_is_valid_enum (klass)) {
		mono_class_set_failure (klass, MONO_EXCEPTION_TYPE_LOAD, NULL);
		mono_raise_exception (mono_get_exception_type_load (tb->name, NULL));
	}

	res = mono_type_get_object (mono_object_domain (tb), &klass->byval_arg);
	g_assert (res != (MonoReflectionType*)tb);

	return res;

failure:
	mono_class_set_failure (klass, MONO_EXCEPTION_TYPE_LOAD, NULL);
	klass->wastypebuilder = TRUE;
	mono_domain_unlock (domain);
	mono_loader_unlock ();
	mono_error_raise_exception (&error);
	return NULL;