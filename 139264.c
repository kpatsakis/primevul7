static struct hlist_head *unix_sockets_unbound(void *addr)
{
	unsigned long hash = (unsigned long)addr;

	hash ^= hash >> 16;
	hash ^= hash >> 8;
	hash %= UNIX_HASH_SIZE;
	return &unix_socket_table[UNIX_HASH_SIZE + hash];
}