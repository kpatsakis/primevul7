request_finished (SoupMessage *msg, gpointer user_data)
{
	SoupClientContext *client = user_data;
	SoupServer *server = client->server;
	SoupSocket *sock = client->sock;

	soup_message_finished (msg);
	g_signal_emit (server,
		       msg->status_code == SOUP_STATUS_IO_ERROR ?
		       signals[REQUEST_ABORTED] : signals[REQUEST_FINISHED],
		       0, msg, client);

	soup_client_context_cleanup (client);
	if (soup_socket_is_connected (sock) && soup_message_is_keepalive (msg)) {
		/* Start a new request */
		start_request (server, client);
	} else {
		soup_socket_disconnect (sock);
		soup_client_context_unref (client);
	}
	g_object_unref (msg);
	g_object_unref (sock);
}