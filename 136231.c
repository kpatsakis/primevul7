gdk_pixbuf__ico_image_stop_load(gpointer data,
				GError **error)
{
	struct ico_progressive_state *context =
	    (struct ico_progressive_state *) data;

        /* FIXME this thing needs to report errors if
         * we have unused image data
         */

	g_return_val_if_fail(context != NULL, TRUE);

	context_free (context);
        return TRUE;
}