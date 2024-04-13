cleanup_refobject_hash (gpointer key, gpointer value, gpointer user_data)
{
	FREE_REFENTRY (key);
}