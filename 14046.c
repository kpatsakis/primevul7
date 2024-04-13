__u32 sctp_generate_tag(const struct sctp_endpoint *ep)
{
	/* I believe that this random number generator complies with RFC1750.
	 * A tag of 0 is reserved for special cases (e.g. INIT).
	 */
	__u32 x;

	do {
		get_random_bytes(&x, sizeof(__u32));
	} while (x == 0);

	return x;
}