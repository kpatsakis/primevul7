new_connection (SoupSocket *listner, SoupSocket *sock, gpointer user_data)
{
	SoupServer *server = user_data;
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (server);
	SoupClientContext *client;

	client = soup_client_context_new (server, g_object_ref (sock));
	priv->clients = g_slist_prepend (priv->clients, client);
	g_signal_connect (sock, "disconnected",
			  G_CALLBACK (socket_disconnected), client);
	start_request (server, client);
}