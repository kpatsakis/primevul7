set_property (GObject *object, guint prop_id,
	      const GValue *value, GParamSpec *pspec)
{
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (object);
	const char *header;

	switch (prop_id) {
	case PROP_PORT:
		priv->port = g_value_get_uint (value);
		break;
	case PROP_INTERFACE:
		if (priv->interface)
			g_object_unref (priv->interface);
		priv->interface = g_value_get_object (value);
		if (priv->interface)
			g_object_ref (priv->interface);
		break;
	case PROP_SSL_CERT_FILE:
		priv->ssl_cert_file =
			g_strdup (g_value_get_string (value));
		break;
	case PROP_SSL_KEY_FILE:
		priv->ssl_key_file =
			g_strdup (g_value_get_string (value));
		break;
	case PROP_ASYNC_CONTEXT:
		priv->async_context = g_value_get_pointer (value);
		if (priv->async_context)
			g_main_context_ref (priv->async_context);
		break;
	case PROP_RAW_PATHS:
		priv->raw_paths = g_value_get_boolean (value);
		break;
	case PROP_SERVER_HEADER:
		g_free (priv->server_header);
		header = g_value_get_string (value);
		if (!header)
			priv->server_header = NULL;
		else if (!*header) {
			priv->server_header =
				g_strdup (SOUP_SERVER_SERVER_HEADER_BASE);
		} else if (g_str_has_suffix (header, " ")) {
			priv->server_header =
				g_strdup_printf ("%s%s", header,
						 SOUP_SERVER_SERVER_HEADER_BASE);
		} else
			priv->server_header = g_strdup (header);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}