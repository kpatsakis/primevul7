soup_client_context_get_address (SoupClientContext *client)
{
	g_return_val_if_fail (client != NULL, NULL);

	return soup_socket_get_remote_address (client->sock);
}