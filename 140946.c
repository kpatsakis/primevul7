soup_client_context_cleanup (SoupClientContext *client)
{
	if (client->auth_domain) {
		g_object_unref (client->auth_domain);
		client->auth_domain = NULL;
	}
	if (client->auth_user) {
		g_free (client->auth_user);
		client->auth_user = NULL;
	}
	client->msg = NULL;
}