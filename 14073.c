const union sctp_addr *sctp_source(const struct sctp_chunk *chunk)
{
	/* If we have a known transport, use that.  */
	if (chunk->transport) {
		return &chunk->transport->ipaddr;
	} else {
		/* Otherwise, extract it from the IP header.  */
		return &chunk->source;
	}
}