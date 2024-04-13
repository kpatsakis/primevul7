e_mail_parser_class_init (EMailParserClass *class)
{
	GObjectClass *object_class;

	parent_class = g_type_class_peek_parent (class);
	g_type_class_add_private (class, sizeof (EMailParserPrivate));

	object_class = G_OBJECT_CLASS (class);
	object_class->finalize = e_mail_parser_finalize;
	object_class->set_property = e_mail_parser_set_property;
	object_class->get_property = e_mail_parser_get_property;

	g_object_class_install_property (
		object_class,
		PROP_SESSION,
		g_param_spec_object (
			"session",
			"Camel Session",
			NULL,
			CAMEL_TYPE_SESSION,
			G_PARAM_READWRITE |
			G_PARAM_CONSTRUCT_ONLY));
}