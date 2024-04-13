get_field_on_inst_generic_type (MonoClassField *field)
{
	MonoClass *class, *gtd;
	MonoDynamicGenericClass *dgclass;
	int field_index;

	g_assert (is_field_on_inst (field));

	dgclass = (MonoDynamicGenericClass*)field->parent->generic_class;

	if (field >= dgclass->fields && field - dgclass->fields < dgclass->count_fields) {
		field_index = field - dgclass->fields;
		return dgclass->field_generic_types [field_index];		
	}

	class = field->parent;
	gtd = class->generic_class->container_class;

	if (field >= class->fields && field - class->fields < class->field.count) {
		field_index = field - class->fields;
		return gtd->fields [field_index].type;
	}

	g_assert_not_reached ();
	return 0;
}