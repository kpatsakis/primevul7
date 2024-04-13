fixup_method (MonoReflectionILGen *ilgen, gpointer value, MonoDynamicImage *assembly)
{
	guint32 code_idx = GPOINTER_TO_UINT (value);
	MonoReflectionILTokenInfo *iltoken;
	MonoReflectionFieldBuilder *field;
	MonoReflectionCtorBuilder *ctor;
	MonoReflectionMethodBuilder *method;
	MonoReflectionTypeBuilder *tb;
	MonoReflectionArrayMethod *am;
	guint32 i, idx = 0;
	unsigned char *target;

	for (i = 0; i < ilgen->num_token_fixups; ++i) {
		iltoken = (MonoReflectionILTokenInfo *)mono_array_addr_with_size (ilgen->token_fixups, sizeof (MonoReflectionILTokenInfo), i);
		target = (guchar*)assembly->code.data + code_idx + iltoken->code_pos;
		switch (target [3]) {
		case MONO_TABLE_FIELD:
			if (!strcmp (iltoken->member->vtable->klass->name, "FieldBuilder")) {
				field = (MonoReflectionFieldBuilder *)iltoken->member;
				idx = field->table_idx;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MonoField")) {
				MonoClassField *f = ((MonoReflectionField*)iltoken->member)->field;
				idx = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->field_to_table_idx, f));
			} else {
				g_assert_not_reached ();
			}
			break;
		case MONO_TABLE_METHOD:
			if (!strcmp (iltoken->member->vtable->klass->name, "MethodBuilder")) {
				method = (MonoReflectionMethodBuilder *)iltoken->member;
				idx = method->table_idx;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "ConstructorBuilder")) {
				ctor = (MonoReflectionCtorBuilder *)iltoken->member;
				idx = ctor->table_idx;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MonoMethod") || 
					   !strcmp (iltoken->member->vtable->klass->name, "MonoCMethod")) {
				MonoMethod *m = ((MonoReflectionMethod*)iltoken->member)->method;
				idx = GPOINTER_TO_UINT (g_hash_table_lookup (assembly->method_to_table_idx, m));
			} else {
				g_assert_not_reached ();
			}
			break;
		case MONO_TABLE_TYPEDEF:
			if (strcmp (iltoken->member->vtable->klass->name, "TypeBuilder"))
				g_assert_not_reached ();
			tb = (MonoReflectionTypeBuilder *)iltoken->member;
			idx = tb->table_idx;
			break;
		case MONO_TABLE_MEMBERREF:
			if (!strcmp (iltoken->member->vtable->klass->name, "MonoArrayMethod")) {
				am = (MonoReflectionArrayMethod*)iltoken->member;
				idx = am->table_idx;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MonoMethod") ||
				   !strcmp (iltoken->member->vtable->klass->name, "MonoCMethod") ||
				   !strcmp (iltoken->member->vtable->klass->name, "MonoGenericMethod") ||
				   !strcmp (iltoken->member->vtable->klass->name, "MonoGenericCMethod")) {
				MonoMethod *m = ((MonoReflectionMethod*)iltoken->member)->method;
				g_assert (m->klass->generic_class || m->klass->generic_container);
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "FieldBuilder")) {
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MonoField")) {
				MonoClassField *f = ((MonoReflectionField*)iltoken->member)->field;
				g_assert (is_field_on_inst (f));
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MethodBuilder") ||
					!strcmp (iltoken->member->vtable->klass->name, "ConstructorBuilder")) {
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "FieldOnTypeBuilderInst")) {
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MethodOnTypeBuilderInst")) {
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "ConstructorOnTypeBuilderInst")) {
				continue;
			} else {
				g_assert_not_reached ();
			}
			break;
		case MONO_TABLE_METHODSPEC:
			if (!strcmp (iltoken->member->vtable->klass->name, "MonoGenericMethod")) {
				MonoMethod *m = ((MonoReflectionMethod*)iltoken->member)->method;
				g_assert (mono_method_signature (m)->generic_param_count);
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MethodBuilder")) {
				continue;
			} else if (!strcmp (iltoken->member->vtable->klass->name, "MethodOnTypeBuilderInst")) {
				continue;
			} else {
				g_assert_not_reached ();
			}
			break;
		default:
			g_error ("got unexpected table 0x%02x in fixup", target [3]);
		}
		target [0] = idx & 0xff;
		target [1] = (idx >> 8) & 0xff;
		target [2] = (idx >> 16) & 0xff;
	}
}