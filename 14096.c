__u32 sctp_generate_tsn(const struct sctp_endpoint *ep)
{
	__u32 retval;

	get_random_bytes(&retval, sizeof(__u32));
	return retval;
}