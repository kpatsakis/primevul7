int sldns_str2wire_aaaa_buf(const char* str, uint8_t* rd, size_t* len)
{
#ifdef AF_INET6
	uint8_t address[LDNS_IP6ADDRLEN + 1];
	if(inet_pton(AF_INET6, (char*)str, address) != 1)
		return LDNS_WIREPARSE_ERR_SYNTAX_IP6;
	if(*len < LDNS_IP6ADDRLEN)
		return LDNS_WIREPARSE_ERR_BUFFER_TOO_SMALL;
	memmove(rd, address, LDNS_IP6ADDRLEN);
	*len = LDNS_IP6ADDRLEN;
	return LDNS_WIREPARSE_ERR_OK;
#else
	return LDNS_WIREPARSE_ERR_NOT_IMPL;
#endif
}