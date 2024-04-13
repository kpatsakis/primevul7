socket_disconnected (SoupSocket *sock, SoupClientContext *client)
{
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (client->server);

	priv->clients = g_slist_remove (priv->clients, client);
	g_signal_handlers_disconnect_by_func (sock, socket_disconnected, client);
	g_object_unref (sock);
}