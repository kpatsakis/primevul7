context_free (struct ico_progressive_state *context)
{
	g_free (context->LineBuf);
	context->LineBuf = NULL;
	g_free (context->HeaderBuf);
	g_list_free_full (context->entries, g_free);
	if (context->pixbuf)
		g_object_unref (context->pixbuf);

	g_free (context);
}