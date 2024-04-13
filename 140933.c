soup_client_context_get_socket (SoupClientContext *client)
{
	g_return_val_if_fail (client != NULL, NULL);

	return client->sock;
}