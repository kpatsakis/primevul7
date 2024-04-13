void sctp_chunk_free(struct sctp_chunk *chunk)
{
	/* Release our reference on the message tracker. */
	if (chunk->msg)
		sctp_datamsg_put(chunk->msg);

	sctp_chunk_put(chunk);
}