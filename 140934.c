soup_server_get_async_context (SoupServer *server)
{
	SoupServerPrivate *priv;

	g_return_val_if_fail (SOUP_IS_SERVER (server), NULL);
	priv = SOUP_SERVER_GET_PRIVATE (server);

	return priv->async_context;
}