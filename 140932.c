finalize (GObject *object)
{
	SoupServer *server = SOUP_SERVER (object);
	SoupServerPrivate *priv = SOUP_SERVER_GET_PRIVATE (server);
	GSList *iter;

	if (priv->interface)
		g_object_unref (priv->interface);

	g_free (priv->ssl_cert_file);
	g_free (priv->ssl_key_file);
	if (priv->ssl_creds)
		soup_ssl_free_server_credentials (priv->ssl_creds);

	g_free (priv->server_header);

	if (priv->listen_sock)
		g_object_unref (priv->listen_sock);

	while (priv->clients) {
		SoupClientContext *client = priv->clients->data;
		SoupSocket *sock = g_object_ref (client->sock);

		priv->clients = g_slist_remove (priv->clients, client);

		if (client->msg) {
			soup_message_set_status (client->msg, SOUP_STATUS_IO_ERROR);
			soup_message_io_finished (client->msg);
		}

		soup_socket_disconnect (sock);
		g_object_unref (sock);
	}

	if (priv->default_handler)
		free_handler (priv->default_handler);
	soup_path_map_free (priv->handlers);

	for (iter = priv->auth_domains; iter; iter = iter->next)
		g_object_unref (iter->data);
	g_slist_free (priv->auth_domains);

	if (priv->loop)
		g_main_loop_unref (priv->loop);
	if (priv->async_context)
		g_main_context_unref (priv->async_context);

	G_OBJECT_CLASS (soup_server_parent_class)->finalize (object);
}