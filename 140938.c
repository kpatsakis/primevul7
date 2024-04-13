soup_client_context_ref (SoupClientContext *client)
{
	client->ref_count++;
	return client;
}