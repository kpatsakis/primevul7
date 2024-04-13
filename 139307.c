	__acquires(unix_table_lock)
{
	spin_lock(&unix_table_lock);

	if (!*pos)
		return SEQ_START_TOKEN;

	if (get_bucket(*pos) >= ARRAY_SIZE(unix_socket_table))
		return NULL;

	return unix_next_socket(seq, NULL, pos);
}