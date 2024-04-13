mono_image_create_token (MonoDynamicImage *assembly, MonoObject *obj, 
						 gboolean create_methodspec, gboolean register_token)
{
	MonoClass *klass;
	guint32 token = 0;

	klass = obj->vtable->klass;

	/* Check for user defined reflection objects */
	/* TypeDelegator is the only corlib type which doesn't look like a MonoReflectionType */
	if (klass->image != mono_defaults.corlib || (strcmp (klass->name, "TypeDelegator") == 0))
		mono_raise_exception (mono_get_exception_not_supported ("User defined subclasses of System.Type are not yet supported")); \

	if (strcmp (klass->name, "MethodBuilder") == 0) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder *)obj;
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)mb->type;

		if (tb->module->dynamic_image == assembly && !tb->generic_params && !mb->generic_params)
			token = mb->table_idx | MONO_TOKEN_METHOD_DEF;
		else
			token = mono_image_get_methodbuilder_token (assembly, mb, create_methodspec);
		/*g_print ("got token 0x%08x for %s\n", token, mono_string_to_utf8 (mb->name));*/
	} else if (strcmp (klass->name, "ConstructorBuilder") == 0) {
		MonoReflectionCtorBuilder *mb = (MonoReflectionCtorBuilder *)obj;
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder*)mb->type;

		if (tb->module->dynamic_image == assembly && !tb->generic_params)
			token = mb->table_idx | MONO_TOKEN_METHOD_DEF;
		else
			token = mono_image_get_ctorbuilder_token (assembly, mb);
		/*g_print ("got token 0x%08x for %s\n", token, mono_string_to_utf8 (mb->name));*/
	} else if (strcmp (klass->name, "FieldBuilder") == 0) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder *)obj;
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)fb->typeb;
		if (tb->generic_params) {
			token = mono_image_get_generic_field_token (assembly, fb);
		} else {
			if ((tb->module->dynamic_image == assembly)) {
				token = fb->table_idx | MONO_TOKEN_FIELD_DEF;
			} else {
				token = mono_image_get_fieldref_token (assembly, (MonoObject*)fb, fb->handle);
			}
		}
	} else if (strcmp (klass->name, "TypeBuilder") == 0) {
		MonoReflectionTypeBuilder *tb = (MonoReflectionTypeBuilder *)obj;
		token = tb->table_idx | MONO_TOKEN_TYPE_DEF;
	} else if (strcmp (klass->name, "MonoType") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType *)obj);
		MonoClass *mc = mono_class_from_mono_type (type);
		if (!mono_class_init (mc))
			mono_raise_exception (mono_class_get_exception_for_failure (mc));

		token = mono_metadata_token_from_dor (
			mono_image_typedef_or_ref_full (assembly, type, mc->generic_container == NULL));
	} else if (strcmp (klass->name, "GenericTypeParameterBuilder") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType *)obj);
		token = mono_metadata_token_from_dor (
			mono_image_typedef_or_ref (assembly, type));
	} else if (strcmp (klass->name, "MonoGenericClass") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType *)obj);
		token = mono_metadata_token_from_dor (
			mono_image_typedef_or_ref (assembly, type));
	} else if (strcmp (klass->name, "MonoCMethod") == 0 ||
		   strcmp (klass->name, "MonoMethod") == 0 ||
		   strcmp (klass->name, "MonoGenericMethod") == 0 ||
		   strcmp (klass->name, "MonoGenericCMethod") == 0) {
		MonoReflectionMethod *m = (MonoReflectionMethod *)obj;
		if (m->method->is_inflated) {
			if (create_methodspec)
				token = mono_image_get_methodspec_token (assembly, m->method);
			else
				token = mono_image_get_inflated_method_token (assembly, m->method);
		} else if ((m->method->klass->image == &assembly->image) &&
			 !m->method->klass->generic_class) {
			static guint32 method_table_idx = 0xffffff;
			if (m->method->klass->wastypebuilder) {
				/* we use the same token as the one that was assigned
				 * to the Methodbuilder.
				 * FIXME: do the equivalent for Fields.
				 */
				token = m->method->token;
			} else {
				/*
				 * Each token should have a unique index, but the indexes are
				 * assigned by managed code, so we don't know about them. An
				 * easy solution is to count backwards...
				 */
				method_table_idx --;
				token = MONO_TOKEN_METHOD_DEF | method_table_idx;
			}
		} else {
			token = mono_image_get_methodref_token (assembly, m->method, create_methodspec);
		}
		/*g_print ("got token 0x%08x for %s\n", token, m->method->name);*/
	} else if (strcmp (klass->name, "MonoField") == 0) {
		MonoReflectionField *f = (MonoReflectionField *)obj;
		if ((f->field->parent->image == &assembly->image) && !is_field_on_inst (f->field)) {
			static guint32 field_table_idx = 0xffffff;
			field_table_idx --;
			token = MONO_TOKEN_FIELD_DEF | field_table_idx;
		} else {
			token = mono_image_get_fieldref_token (assembly, (MonoObject*)f, f->field);
		}
		/*g_print ("got token 0x%08x for %s\n", token, f->field->name);*/
	} else if (strcmp (klass->name, "MonoArrayMethod") == 0) {
		MonoReflectionArrayMethod *m = (MonoReflectionArrayMethod *)obj;
		token = mono_image_get_array_token (assembly, m);
	} else if (strcmp (klass->name, "SignatureHelper") == 0) {
		MonoReflectionSigHelper *s = (MonoReflectionSigHelper*)obj;
		token = MONO_TOKEN_SIGNATURE | mono_image_get_sighelper_token (assembly, s);
	} else if (strcmp (klass->name, "EnumBuilder") == 0) {
		MonoType *type = mono_reflection_type_get_handle ((MonoReflectionType *)obj);
		token = mono_metadata_token_from_dor (
			mono_image_typedef_or_ref (assembly, type));
	} else if (strcmp (klass->name, "FieldOnTypeBuilderInst") == 0) {
		MonoReflectionFieldOnTypeBuilderInst *f = (MonoReflectionFieldOnTypeBuilderInst*)obj;
		token = mono_image_get_field_on_inst_token (assembly, f);
	} else if (strcmp (klass->name, "ConstructorOnTypeBuilderInst") == 0) {
		MonoReflectionCtorOnTypeBuilderInst *c = (MonoReflectionCtorOnTypeBuilderInst*)obj;
		token = mono_image_get_ctor_on_inst_token (assembly, c, create_methodspec);
	} else if (strcmp (klass->name, "MethodOnTypeBuilderInst") == 0) {
		MonoReflectionMethodOnTypeBuilderInst *m = (MonoReflectionMethodOnTypeBuilderInst*)obj;
		token = mono_image_get_method_on_inst_token (assembly, m, create_methodspec);
	} else if (is_sre_array (klass) || is_sre_byref (klass) || is_sre_pointer (klass)) {
		MonoReflectionType *type = (MonoReflectionType *)obj;
		token = mono_metadata_token_from_dor (
				mono_image_typedef_or_ref (assembly, mono_reflection_type_get_handle (type)));
	} else {
		g_error ("requested token for %s\n", klass->name);
	}

	if (register_token)
		mono_image_register_token (assembly, token, obj);

	return token;
}