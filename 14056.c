void sctp_init_addrs(struct sctp_chunk *chunk, union sctp_addr *src,
		     union sctp_addr *dest)
{
	memcpy(&chunk->source, src, sizeof(union sctp_addr));
	memcpy(&chunk->dest, dest, sizeof(union sctp_addr));
}