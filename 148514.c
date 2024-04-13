pk_transaction_dbus_return (GDBusMethodInvocation *context, const GError *error)
{
	/* not set inside the test suite */
	if (context == NULL) {
		if (error != NULL)
			g_warning ("context null, and error: %s", error->message);
		return;
	}
	if (error != NULL)
		g_dbus_method_invocation_return_gerror (context, error);
	else
		g_dbus_method_invocation_return_value (context, NULL);
}