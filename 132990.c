exten_payload_size(
	const struct exten * ep)
{
	typedef const u_char *BPTR;
	
	size_t extn_size;
	size_t data_size;
	size_t head_size;

	data_size = 0;
	if (NULL != ep) {
		head_size = (BPTR)(&ep->vallen + 1) - (BPTR)ep;
		extn_size = (uint16_t)(ntohl(ep->opcode) & 0x0000ffff);
		if (extn_size >= head_size) {
			data_size = (uint32_t)ntohl(ep->vallen);
			if (data_size > extn_size - head_size)
				data_size = ~(size_t)0u;
		}
	}
	return (u_int)data_size;
}