soup_server_get_port (SoupServer *server)
{
	g_return_val_if_fail (SOUP_IS_SERVER (server), 0);

	return SOUP_SERVER_GET_PRIVATE (server)->port;
}