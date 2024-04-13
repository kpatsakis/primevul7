soup_server_disconnect (SoupServer *server)
{
	SoupServerPrivate *priv;

	g_return_if_fail (SOUP_IS_SERVER (server));
	priv = SOUP_SERVER_GET_PRIVATE (server);

	soup_server_quit (server);

	if (priv->listen_sock) {
		soup_socket_disconnect (priv->listen_sock);
		g_object_unref (priv->listen_sock);
		priv->listen_sock = NULL;
	}
}