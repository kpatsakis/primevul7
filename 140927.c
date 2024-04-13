get_property (GObject *object, guint prop_id,
	      GValue *value, GParamSpec *pspec)
{
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_PORT:
		g_value_set_uint (value, priv->port);
		break;
	case PROP_INTERFACE:
		g_value_set_object (value, priv->interface);
		break;
	case PROP_SSL_CERT_FILE:
		g_value_set_string (value, priv->ssl_cert_file);
		break;
	case PROP_SSL_KEY_FILE:
		g_value_set_string (value, priv->ssl_key_file);
		break;
	case PROP_ASYNC_CONTEXT:
		g_value_set_pointer (value, priv->async_context ? g_main_context_ref (priv->async_context) : NULL);
		break;
	case PROP_RAW_PATHS:
		g_value_set_boolean (value, priv->raw_paths);
		break;
	case PROP_SERVER_HEADER:
		g_value_set_string (value, priv->server_header);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}