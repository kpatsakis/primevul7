static void
get_field_name_and_type (MonoObject *field, char **name, MonoType **type)
{
	MonoClass *klass = mono_object_class (field);
	if (strcmp (klass->name, "FieldBuilder") == 0) {
		MonoReflectionFieldBuilder *fb = (MonoReflectionFieldBuilder *)field;
		*name = mono_string_to_utf8 (fb->name);
		*type = mono_reflection_type_get_handle ((MonoReflectionType*)fb->type);
	} else {
		MonoReflectionField *f = (MonoReflectionField *)field;
		*name = g_strdup (mono_field_get_name (f->field));
		*type = f->field->type;
	}