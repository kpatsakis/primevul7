free_handler (SoupServerHandler *hand)
{
	g_free (hand->path);
	g_slice_free (SoupServerHandler, hand);
}