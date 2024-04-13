static MonoClassField*
fieldbuilder_to_mono_class_field (MonoClass *klass, MonoReflectionFieldBuilder* fb)
{
	MonoClassField *field;
	MonoType *custom;

	field = g_new0 (MonoClassField, 1);

	field->name = mono_string_to_utf8 (fb->name);
	if (fb->attrs || fb->modreq || fb->modopt) {
		field->type = mono_metadata_type_dup (NULL, mono_reflection_type_get_handle ((MonoReflectionType*)fb->type));
		field->type->attrs = fb->attrs;

		g_assert (klass->image->dynamic);
		custom = add_custom_modifiers ((MonoDynamicImage*)klass->image, field->type, fb->modreq, fb->modopt);
		g_free (field->type);
		field->type = custom;
	} else {
		field->type = mono_reflection_type_get_handle ((MonoReflectionType*)fb->type);
	}
	if (fb->offset != -1)
		field->offset = fb->offset;
	field->parent = klass;
	mono_save_custom_attrs (klass->image, field, fb->cattrs);

	// FIXME: Can't store fb->def_value/RVA, is it needed for field_on_insts ?

	return field;