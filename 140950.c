soup_client_context_get_host (SoupClientContext *client)
{
	SoupAddress *address;

	address = soup_client_context_get_address (client);
	return soup_address_get_physical (address);
}