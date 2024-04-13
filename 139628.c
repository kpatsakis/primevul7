e_mail_parser_get_type (void)
{
	static GType type = 0;

	if (G_UNLIKELY (type == 0)) {
		static const GTypeInfo type_info = {
			sizeof (EMailParserClass),
			(GBaseInitFunc) e_mail_parser_base_init,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) e_mail_parser_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,  /* class_data */
			sizeof (EMailParser),
			0,     /* n_preallocs */
			(GInstanceInitFunc) e_mail_parser_init,
			NULL   /* value_table */
		};

		type = g_type_register_static (
			G_TYPE_OBJECT, "EMailParser",
			&type_info, 0);
	}

	return type;
}