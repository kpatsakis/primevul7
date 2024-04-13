soup_server_get_handler (SoupServer *server, const char *path)
{
	SoupServerPrivate *priv;
	SoupServerHandler *hand;

	g_return_val_if_fail (SOUP_IS_SERVER (server), NULL);
	priv = SOUP_SERVER_GET_PRIVATE (server);

	if (path) {
		hand = soup_path_map_lookup (priv->handlers, path);
		if (hand)
			return hand;
		if (!strcmp (path, "*"))
			return NULL;
	}
	return priv->default_handler;
}