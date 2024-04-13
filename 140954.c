call_handler (SoupMessage *req, SoupClientContext *client)
{
	SoupServer *server = client->server;
	SoupServerHandler *hand;
	SoupURI *uri;

	g_signal_emit (server, signals[REQUEST_READ], 0, req, client);

	if (req->status_code != 0)
		return;

	uri = soup_message_get_uri (req);
	hand = soup_server_get_handler (server, uri->path);
	if (!hand) {
		soup_message_set_status (req, SOUP_STATUS_NOT_FOUND);
		return;
	}

	if (hand->callback) {
		GHashTable *form_data_set;

		if (uri->query)
			form_data_set = soup_form_decode (uri->query);
		else
			form_data_set = NULL;

		/* Call method handler */
		(*hand->callback) (server, req,
				   uri->path, form_data_set,
				   client, hand->user_data);

		if (form_data_set)
			g_hash_table_destroy (form_data_set);
	}
}