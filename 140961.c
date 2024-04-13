soup_client_context_new (SoupServer *server, SoupSocket *sock)
{
	SoupClientContext *client = g_slice_new0 (SoupClientContext);

	client->server = server;
	client->sock = sock;
	client->ref_count = 1;

	return client;
}