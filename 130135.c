mono_reflection_get_token (MonoObject *obj)
{
	MonoClass *klass;
	guint32 token = 0;

	klass = obj->vtable->klass;

	if (strcmp (klass->name, "MethodBuilder") == 0) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder *)obj;

		token = mb->table_idx | MONO_TOKEN_METHOD_DEF;
	} else if (strcmp (klass->name, "ConstructorBuilder") == 0) {
		MonoReflectionCtorBuilder *mb = (MonoReflectionCtorBuilder *)obj;

		token = mb->table_idx | MONO_TOKEN_METHOD_DEF;
	} else if (strcmp (klass->name, "FieldBuilder") == 0) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder *)obj;

		/* Call mono_image_create_token so the object gets added to the tokens hash table */
		token = mono_image_create_token (((MonoReflectionTypeBuilder*)fb->typeb)->module->dynamic_image, obj, FALSE, TRUE);
	} else if (strcmp (klass->name, "TypeBuilder") == 0) {
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)obj;
		token = tb->table_idx | MONO_TOKEN_TYPE_DEF;
	} else if (strcmp (klass->name, "MonoType") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType*)obj);
		MonoClass *mc = mono_class_from_mono_type (type);
		if (!mono_class_init (mc))
			mono_raise_exception (mono_class_get_exception_for_failure (mc));

		token = mc->type_token;
	} else if (strcmp (klass->name, "MonoCMethod") == 0 ||
		   strcmp (klass->name, "MonoMethod") == 0 ||
		   strcmp (klass->name, "MonoGenericMethod") == 0 ||
		   strcmp (klass->name, "MonoGenericCMethod") == 0) {
		MonoReflectionMethod *m = (MonoReflectionMethod *)obj;
		if (m->method->is_inflated) {
			MonoMethodInflated *inflated = (MonoMethodInflated *) m->method;
			return inflated->declaring->token;
		} else {
			token = m->method->token;
		}
	} else if (strcmp (klass->name, "MonoField") == 0) {
		MonoReflectionField *f = (MonoReflectionField*)obj;

		if (is_field_on_inst (f->field)) {
			MonoDynamicGenericClass *dgclass = (MonoDynamicGenericClass*)f->field->parent->generic_class;
			int field_index = f->field - dgclass->fields;
			MonoObject *obj;

			g_assert (field_index >= 0 && field_index < dgclass->count_fields);
			obj = dgclass->field_objects [field_index];
			return mono_reflection_get_token (obj);
		}
		token = mono_class_get_field_token (f->field);
	} else if (strcmp (klass->name, "MonoProperty") == 0) {
		MonoReflectionProperty *p = (MonoReflectionProperty*)obj;

		token = mono_class_get_property_token (p->property);
	} else if (strcmp (klass->name, "MonoEvent") == 0) {
		MonoReflectionMonoEvent *p = (MonoReflectionMonoEvent*)obj;

		token = mono_class_get_event_token (p->event);
	} else if (strcmp (klass->name, "ParameterInfo") == 0) {
		MonoReflectionParameter *p = (MonoReflectionParameter*)obj;
		MonoClass *member_class = mono_object_class (p->MemberImpl);
		g_assert (mono_class_is_reflection_method_or_constructor (member_class));

		token = mono_method_get_param_token (((MonoReflectionMethod*)p->MemberImpl)->method, p->PositionImpl);
	} else if (strcmp (klass->name, "Module") == 0 || strcmp (klass->name, "MonoModule") == 0) {
		MonoReflectionModule *m = (MonoReflectionModule*)obj;

		token = m->token;
	} else if (strcmp (klass->name, "Assembly") == 0 || strcmp (klass->name, "MonoAssembly") == 0) {
		token = mono_metadata_make_token (MONO_TABLE_ASSEMBLY, 1);
	} else {
		gchar *msg = g_strdup_printf ("MetadataToken is not supported for type '%s.%s'", klass->name_space, klass->name);
		MonoException *ex = mono_get_exception_not_implemented (msg);
		g_free (msg);
		mono_raise_exception (ex);
	}

	return token;
}