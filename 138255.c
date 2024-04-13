_g_enum_type_get_value (GType enum_type,
			int   value)
{
	GEnumClass *class;
	GEnumValue *enum_value;

	class = G_ENUM_CLASS (g_type_class_ref (enum_type));
	enum_value = g_enum_get_value (class, value);
	g_type_class_unref (class);

	return enum_value;
}