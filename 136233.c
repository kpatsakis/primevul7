gdk_pixbuf__ico_image_begin_load(GdkPixbufModuleSizeFunc size_func,
                                 GdkPixbufModulePreparedFunc prepared_func,
				 GdkPixbufModuleUpdatedFunc updated_func,
				 gpointer user_data,
                                 GError **error)
{
	struct ico_progressive_state *context;

	context = g_new0(struct ico_progressive_state, 1);
	context->size_func = size_func;
	context->prepared_func = prepared_func;
	context->updated_func = updated_func;
	context->user_data = user_data;

	context->HeaderSize = 54;
	context->HeaderBuf = g_try_malloc(14 + 40 + 4*256 + 512);
	if (!context->HeaderBuf) {
		g_free (context);
		g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                     _("Not enough memory to load ICO file"));
		return NULL;
	}
	/* 4*256 for the colormap */
	context->BytesInHeaderBuf = 14 + 40 + 4*256 + 512 ;
	context->HeaderDone = 0;

	context->LineWidth = 0;
	context->LineBuf = NULL;
	context->LineDone = 0;
	context->Lines = 0;

	context->Type = 0;

	memset(&context->Header, 0, sizeof(struct headerpair));


	context->pixbuf = NULL;


	return (gpointer) context;
}