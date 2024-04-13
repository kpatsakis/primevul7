gdk_pixbuf__ico_image_load_increment(gpointer data,
                                     const guchar * buf,
                                     guint size,
                                     GError **error)
{
	struct ico_progressive_state *context =
	    (struct ico_progressive_state *) data;

	gint BytesToCopy;

	while (size > 0) {
		g_assert(context->LineDone >= 0);
		if (context->HeaderDone < context->HeaderSize) {	/* We still 
									   have headerbytes to do */
			BytesToCopy =
			    context->HeaderSize - context->HeaderDone;
			if (BytesToCopy > size)
				BytesToCopy = size;

			memmove(context->HeaderBuf + context->HeaderDone,
			       buf, BytesToCopy);

			size -= BytesToCopy;
			buf += BytesToCopy;
			context->HeaderDone += BytesToCopy;
		} 
		else {
			BytesToCopy =
			    context->LineWidth - context->LineDone;
			if (BytesToCopy > size)
				BytesToCopy = size;

			if (BytesToCopy > 0) {
				memmove(context->LineBuf +
				       context->LineDone, buf,
				       BytesToCopy);

				size -= BytesToCopy;
				buf += BytesToCopy;
				context->LineDone += BytesToCopy;
			}
			if ((context->LineDone >= context->LineWidth) &&
			    (context->LineWidth > 0))
				OneLine(context);


		}

		if (context->HeaderDone >= 6 && context->pixbuf == NULL) {
			GError *decode_err = NULL;
			DecodeHeader(context->HeaderBuf,
				     context->HeaderDone, context, &decode_err);
			if (context->LineBuf != NULL && context->LineWidth == 0)
				return TRUE;

			if (decode_err) {
				g_propagate_error (error, decode_err);
				return FALSE;
			}
		}
	}

	return TRUE;
}