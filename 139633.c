shell_gone_cb (gpointer user_data,
	       GObject *gone_extension_registry)
{
	EMailParserClass *class = user_data;

	g_return_if_fail (class != NULL);

	g_clear_object (&class->extension_registry);
}