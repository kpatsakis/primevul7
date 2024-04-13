scrollback_close (session *sess)
{
	g_clear_object (&sess->scrollfile);
}