soup_client_context_unref (SoupClientContext *client)
{
	if (--client->ref_count == 0) {
		soup_client_context_cleanup (client);
		g_slice_free (SoupClientContext, client);
	}
}