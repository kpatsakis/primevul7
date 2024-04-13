soup_client_context_get_auth_domain (SoupClientContext *client)
{
	g_return_val_if_fail (client != NULL, NULL);

	return client->auth_domain;
}