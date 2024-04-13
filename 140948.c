soup_server_init (SoupServer *server)
{
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (server);

	priv->handlers = soup_path_map_new ((GDestroyNotify)free_handler);
}