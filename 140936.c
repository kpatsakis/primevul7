unregister_handler (SoupServerHandler *handler)
{
	if (handler->destroy)
		handler->destroy (handler->user_data);
}