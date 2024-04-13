soup_server_remove_auth_domain (SoupServer *server, SoupAuthDomain *auth_domain)
{
	SoupServerPrivate *priv;

	g_return_if_fail (SOUP_IS_SERVER (server));
	priv = SOUP_SERVER_GET_PRIVATE (server);

	priv->auth_domains = g_slist_remove (priv->auth_domains, auth_domain);
	g_object_unref (auth_domain);
}