soup_server_remove_handler (SoupServer *server, const char *path)
{
	SoupServerPrivate *priv;
	SoupServerHandler *hand;

	g_return_if_fail (SOUP_IS_SERVER (server));
	priv = SOUP_SERVER_GET_PRIVATE (server);

	if (!path || !*path || !strcmp (path, "/")) {
		if (priv->default_handler) {
			unregister_handler (priv->default_handler);
			free_handler (priv->default_handler);
			priv->default_handler = NULL;
		}
		return;
	}

	hand = soup_path_map_lookup (priv->handlers, path);
	if (hand && !strcmp (path, hand->path)) {
		unregister_handler (hand);
		soup_path_map_remove (priv->handlers, path);
	}
}