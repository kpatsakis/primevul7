soup_server_add_handler (SoupServer            *server,
			 const char            *path,
			 SoupServerCallback     callback,
			 gpointer               user_data,
			 GDestroyNotify         destroy)
{
	SoupServerPrivate *priv;
	SoupServerHandler *hand;

	g_return_if_fail (SOUP_IS_SERVER (server));
	g_return_if_fail (callback != NULL);
	priv = SOUP_SERVER_GET_PRIVATE (server);

	/* "" was never documented as meaning the same this as "/",
	 * but it effectively was. We have to special case it now or
	 * otherwise it would match "*" too.
	 */
	if (path && (!*path || !strcmp (path, "/")))
		path = NULL;

	hand = g_slice_new0 (SoupServerHandler);
	hand->path       = g_strdup (path);
	hand->callback   = callback;
	hand->destroy    = destroy;
	hand->user_data  = user_data;

	soup_server_remove_handler (server, path);
	if (path)
		soup_path_map_add (priv->handlers, path, hand);
	else
		priv->default_handler = hand;
}