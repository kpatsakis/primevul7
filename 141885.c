static inline bool addr_match(const void *token1, const void *token2,
			      unsigned int prefixlen)
{
	const __be32 *a1 = token1;
	const __be32 *a2 = token2;
	unsigned int pdw;
	unsigned int pbi;

	pdw = prefixlen >> 5;	  /* num of whole u32 in prefix */
	pbi = prefixlen &  0x1f;  /* num of bits in incomplete u32 in prefix */

	if (pdw)
		if (memcmp(a1, a2, pdw << 2))
			return false;

	if (pbi) {
		__be32 mask;

		mask = htonl((0xffffffff) << (32 - pbi));

		if ((a1[pdw] ^ a2[pdw]) & mask)
			return false;
	}

	return true;
}