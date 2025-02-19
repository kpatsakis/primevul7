static int sctp_process_inv_paramlength(const struct sctp_association *asoc,
					struct sctp_paramhdr *param,
					const struct sctp_chunk *chunk,
					struct sctp_chunk **errp)
{
	/* This is a fatal error.  Any accumulated non-fatal errors are
	 * not reported.
	 */
	if (*errp)
		sctp_chunk_free(*errp);

	/* Create an error chunk and fill it in with our payload. */
	*errp = sctp_make_violation_paramlen(asoc, chunk, param);

	return 0;
}