mono_type_get_object (MonoDomain *domain, MonoType *type)
{
	MonoType *norm_type;
	MonoReflectionType *res;
	MonoClass *klass = mono_class_from_mono_type (type);

	/*we must avoid using @type as it might have come
	 * from a mono_metadata_type_dup and the caller
	 * expects that is can be freed.
	 * Using the right type from 
	 */
	type = klass->byval_arg.byref == type->byref ? &klass->byval_arg : &klass->this_arg;

	/* void is very common */
	if (type->type == MONO_TYPE_VOID && domain->typeof_void)
		return (MonoReflectionType*)domain->typeof_void;

	/*
	 * If the vtable of the given class was already created, we can use
	 * the MonoType from there and avoid all locking and hash table lookups.
	 * 
	 * We cannot do this for TypeBuilders as mono_reflection_create_runtime_class expects
	 * that the resulting object is different.   
	 */
	if (type == &klass->byval_arg && !klass->image->dynamic) {
		MonoVTable *vtable = mono_class_try_get_vtable (domain, klass);
		if (vtable && vtable->type)
			return vtable->type;
	}

	mono_loader_lock (); /*FIXME mono_class_init and mono_class_vtable acquire it*/
	mono_domain_lock (domain);
	if (!domain->type_hash)
		domain->type_hash = mono_g_hash_table_new_type ((GHashFunc)mymono_metadata_type_hash, 
				(GCompareFunc)mymono_metadata_type_equal, MONO_HASH_VALUE_GC);
	if ((res = mono_g_hash_table_lookup (domain->type_hash, type))) {
		mono_domain_unlock (domain);
		mono_loader_unlock ();
		return res;
	}

	/*Types must be normalized so a generic instance of the GTD get's the same inner type.
	 * For example in: Foo<A,B>; Bar<A> : Foo<A, Bar<A>>
	 * The second Bar will be encoded a generic instance of Bar with <A> as parameter.
	 * On all other places, Bar<A> will be encoded as the GTD itself. This is an implementation
	 * artifact of how generics are encoded and should be transparent to managed code so we
	 * need to weed out this diference when retrieving managed System.Type objects.
	 */
	norm_type = mono_type_normalize (type);
	if (norm_type != type) {
		res = mono_type_get_object (domain, norm_type);
		mono_g_hash_table_insert (domain->type_hash, type, res);
		mono_domain_unlock (domain);
		mono_loader_unlock ();
		return res;
	}

	/* This MonoGenericClass hack is no longer necessary. Let's leave it here until we finish with the 2-stage type-builder setup.*/
	if ((type->type == MONO_TYPE_GENERICINST) && type->data.generic_class->is_dynamic && !type->data.generic_class->container_class->wastypebuilder)
		g_assert (0);

	if (!verify_safe_for_managed_space (type)) {
		mono_domain_unlock (domain);
		mono_loader_unlock ();
		mono_raise_exception (mono_get_exception_invalid_operation ("This type cannot be propagated to managed space"));
	}

	if (mono_class_get_ref_info (klass) && !klass->wastypebuilder) {
		gboolean is_type_done = TRUE;
		/* Generic parameters have reflection_info set but they are not finished together with their enclosing type.
		 * We must ensure that once a type is finished we don't return a GenericTypeParameterBuilder.
		 * We can't simply close the types as this will interfere with other parts of the generics machinery.
		*/
		if (klass->byval_arg.type == MONO_TYPE_MVAR || klass->byval_arg.type == MONO_TYPE_VAR) {
			MonoGenericParam *gparam = klass->byval_arg.data.generic_param;

			if (gparam->owner && gparam->owner->is_method) {
				MonoMethod *method = gparam->owner->owner.method;
				if (method && mono_class_get_generic_type_definition (method->klass)->wastypebuilder)
					is_type_done = FALSE;
			} else if (gparam->owner && !gparam->owner->is_method) {
				MonoClass *klass = gparam->owner->owner.klass;
				if (klass && mono_class_get_generic_type_definition (klass)->wastypebuilder)
					is_type_done = FALSE;
			}
		} 

		/* g_assert_not_reached (); */
		/* should this be considered an error condition? */
		if (is_type_done && !type->byref) {
			mono_domain_unlock (domain);
			mono_loader_unlock ();
			return mono_class_get_ref_info (klass);
		}
	}
#ifdef HAVE_SGEN_GC
	res = (MonoReflectionType *)mono_gc_alloc_pinned_obj (mono_class_vtable (domain, mono_defaults.monotype_class), mono_class_instance_size (mono_defaults.monotype_class));
#else
	res = (MonoReflectionType *)mono_object_new (domain, mono_defaults.monotype_class);
#endif
	res->type = type;
	mono_g_hash_table_insert (domain->type_hash, type, res);

	if (type->type == MONO_TYPE_VOID)
		domain->typeof_void = (MonoObject*)res;

	mono_domain_unlock (domain);
	mono_loader_unlock ();
	return res;
}