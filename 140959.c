soup_client_context_get_type (void)
{
	static volatile gsize type_volatile = 0;

	if (g_once_init_enter (&type_volatile)) {
		GType type = g_boxed_type_register_static (
			g_intern_static_string ("SoupClientContext"),
			(GBoxedCopyFunc) soup_client_context_ref,
			(GBoxedFreeFunc) soup_client_context_unref);
		g_once_init_leave (&type_volatile, type);
	}
	return type_volatile;
}