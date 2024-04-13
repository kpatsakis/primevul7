soup_server_is_https (SoupServer *server)
{
	SoupServerPrivate *priv;

	g_return_val_if_fail (SOUP_IS_SERVER (server), 0);
	priv = SOUP_SERVER_GET_PRIVATE (server);

	return (priv->ssl_cert_file && priv->ssl_key_file);
}