forward_equals(const struct Forward *a, const struct Forward *b)
{
	if (strcmp_maybe_null(a->listen_host, b->listen_host) == 0)
		return 0;
	if (a->listen_port != b->listen_port)
		return 0;
	if (strcmp_maybe_null(a->listen_path, b->listen_path) == 0)
		return 0;
	if (strcmp_maybe_null(a->connect_host, b->connect_host) == 0)
		return 0;
	if (a->connect_port != b->connect_port)
		return 0;
	if (strcmp_maybe_null(a->connect_path, b->connect_path) == 0)
		return 0;
	/* allocated_port and handle are not checked */
	return 1;
}