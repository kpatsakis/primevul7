static MonoMethod *
inflate_method (MonoReflectionType *type, MonoObject *obj)
{
	MonoMethod *method;
	MonoClass *gklass;

	MonoClass *type_class = mono_object_class (type);

	if (is_sre_generic_instance (type_class)) {
		MonoReflectionGenericClass *mgc = (MonoReflectionGenericClass*)type;
		gklass = mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)mgc->generic_type));
	} else if (is_sre_type_builder (type_class)) {
		gklass = mono_class_from_mono_type (mono_reflection_type_get_handle (type));
	} else if (type->type) {
		gklass = mono_class_from_mono_type (type->type);
		gklass = mono_class_get_generic_type_definition (gklass);
	} else {
		g_error ("Can't handle type %s", mono_type_get_full_name (mono_object_class (type)));
	}

	if (!strcmp (obj->vtable->klass->name, "MethodBuilder"))
		if (((MonoReflectionMethodBuilder*)obj)->mhandle)
			method = ((MonoReflectionMethodBuilder*)obj)->mhandle;
		else
			method = methodbuilder_to_mono_method (gklass, (MonoReflectionMethodBuilder *) obj);
	else if (!strcmp (obj->vtable->klass->name, "ConstructorBuilder"))
		method = ctorbuilder_to_mono_method (gklass, (MonoReflectionCtorBuilder *) obj);
	else if (!strcmp (obj->vtable->klass->name, "MonoMethod") || !strcmp (obj->vtable->klass->name, "MonoCMethod"))
		method = ((MonoReflectionMethod *) obj)->method;
	else {
		method = NULL; /* prevent compiler warning */
		g_error ("can't handle type %s", obj->vtable->klass->name);
	}

	return inflate_mono_method (mono_class_from_mono_type (mono_reflection_type_get_handle ((MonoReflectionType*)type)), method, obj);