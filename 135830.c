static int handle_one_reflog_ent(unsigned char *osha1, unsigned char *nsha1,
		const char *email, unsigned long timestamp, int tz,
		const char *message, void *cb_data)
{
	handle_one_reflog_commit(osha1, cb_data);
	handle_one_reflog_commit(nsha1, cb_data);
	return 0;
}